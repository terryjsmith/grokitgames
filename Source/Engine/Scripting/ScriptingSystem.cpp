
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>
#include <Core/MessageSystem.h>
#include <Core/World.h>
#include <IO/ResourceSystem.h>
#include <IO/Profiler.h>
#include <Core/Directory.h>
#include <Scripting/ScriptObject.h>
#include <Core/DataRecord.h>

GigaObject* ScriptingSystem::internal_GigaObject_Ctor(MonoObject* obj) {
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    
    // Search first
    GigaObject* newobj = 0;
    auto oi = scriptingSystem->m_objects.begin();
    for(; oi != scriptingSystem->m_objects.end(); oi++) {
        if((*oi)->remote == obj) {
            newobj = (*oi)->local;
            break;
        }
    }
    
    if(newobj) {
        return(newobj);
    }
    
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    
    // Figure out what type it is
    MonoClass* _class = mono_object_get_class(obj);
    
    // Find the cached class description
    MonoClassDesc* cl = 0;
    auto it = scriptingSystem->m_classes.begin();
    for(; it != scriptingSystem->m_classes.end(); it++) {
        if(it->second->_class == _class) {
            cl = it->second;
            break;
        }
    }
    
    GIGA_ASSERT(cl != 0, "Class type not found.");
    
    // Process inheritance to find top-level native C++ class
    Meta::Class* mc = metaSystem->FindClass(cl->name);
    if(mc->native == false) {
        auto ci = mc->inheritsFrom.begin();
        for(; ci != mc->inheritsFrom.end(); ci++) {
            Meta::Class* mci = metaSystem->FindClass(*ci);
            if(mci) {
                if(mci->native == true) {
                    mc = mci;
                }
            }
        }
        
        if(ci == mc->inheritsFrom.end()) mc = metaSystem->FindClass("GigaObject");
    }
    
    // Create a new script object
    newobj = mc->CreateObject();
    
    ScriptObject* cobj = new ScriptObject();
    cobj->local = newobj;
    cobj->remote = obj;
    cobj->classType = cl;
    
    scriptingSystem->m_objects.push_back(cobj);
    
    return(newobj);
}

void ScriptingSystem::AddInternalCall(std::string definition, void* func) {
    mono_add_internal_call(definition.c_str(), (void*)func);
}

void ScriptingSystem::Start() {
    // Load base engine functionality (tie back to engine c++ code)
    MonoImage* image = this->LoadLibrary("Resources/Scripting/GIGAEngine.dll");
    
    // Tie our constructor for new objects
    mono_add_internal_call("GIGA.GigaObject::internal_GigaObject_Ctor", (void*)internal_GigaObject_Ctor);
    
    // Register tie back functions
    RegisterMonoFunctions();
    
    m_cachedTypes[Variant::VAR_BOOL] = mono_get_boolean_class();
    m_cachedTypes[Variant::VAR_INT32] = mono_get_int32_class();
    m_cachedTypes[Variant::VAR_INT64] = mono_get_int64_class();
    m_cachedTypes[Variant::VAR_UINT32] = mono_get_uint32_class();
    m_cachedTypes[Variant::VAR_UINT64] = mono_get_uint64_class();
    m_cachedTypes[Variant::VAR_FLOAT] = mono_get_double_class();
    m_cachedTypes[Variant::VAR_STRING] = mono_get_string_class();
    m_cachedTypes[Variant::VAR_VECTOR2] = mono_class_from_name(image, "GIGA", "Vector2");
    m_cachedTypes[Variant::VAR_VECTOR3] = mono_class_from_name(image, "GIGA", "Vector3");
    m_cachedTypes[Variant::VAR_VECTOR4] = mono_class_from_name(image, "GIGA", "Vector4");
    m_cachedTypes[Variant::VAR_QUATERNION] = mono_class_from_name(image, "GIGA", "Quaternion");
}

void ScriptingSystem::Initialize() {
    std::string cwd = Directory::GetCurrent();
    std::string libDir = cwd + "/Resources/Scripting/lib";
    std::string configDir = cwd + "/Resources/Scripting/etc";
    mono_set_dirs(libDir.c_str(), configDir.c_str());
    mono_config_parse (NULL);

    m_domain = mono_jit_init("GrokItGames");
    
    // Set up mappings
    m_variantMappings["Vector2"] = Variant::VAR_VECTOR2;
    m_variantMappings["Vector3"] = Variant::VAR_VECTOR3;
    m_variantMappings["Vector4"] = Variant::VAR_VECTOR4;
    m_variantMappings["Quaterion"] = Variant::VAR_QUATERNION;
}

MonoImage* ScriptingSystem::LoadLibrary(std::string filename) {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    
    // Load assembly
    MonoAssembly* assembly = mono_domain_assembly_open(m_domain, filename.c_str());
    MonoImage* image = mono_assembly_get_image(assembly);
    
    // Load library function, called at end
    MonoMethod* loadLibrary = 0;
    
    // Load classes
    Array<std::string> classNames;
    const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    
    int rows = mono_table_info_get_rows(table_info);
    for (int i = 0; i < rows; i++) {
        MonoClass* _class = nullptr;
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        _class = mono_class_from_name(image, name_space, name);
        
        if(strcmp(name, "<Module>") == 0) {
            continue;
        }
        
        classNames.push_back(name);
        
        MonoClassDesc* cl = new MonoClassDesc();
        cl->name = name;
        cl->_class = _class;
        
        m_classes[name] = cl;
        
        // Make sure the class name is registered with the meta system
        if(strcmp(name_space, "GIGA") != 0) {
            Meta::Class* mcl = metaSystem->FindClass(name);
            if(mcl == 0) {
                // Create new meta class
                mcl = new Meta::Class();
                mcl->name = name;
                
                // All other objects get registered as generic ScriptObjects
                mcl->RegisterConstructor<ScriptObject>();
                mcl->typeID = metaSystem->GetNextTypeID();
                mcl->singleton = false;
                mcl->native = false;
                
                metaSystem->AddClass(mcl);
            }
        }
        
        // Load functions
        void* iter = NULL;
        MonoMethod* method = mono_class_get_methods(_class, &iter);
        while(method != NULL) {
            std::string funcName = mono_method_full_name(method, 0);
            funcName = funcName.substr(funcName.find(":") + 1);
            
            if(funcName == "LoadLibrary") {
                loadLibrary = method;
            }
            
            MonoMethodDesc* fn = new MonoMethodDesc();
            fn->name = funcName;
            fn->method = method;
            cl->methods[fn->name] = fn;
            
            method = mono_class_get_methods(_class, &iter);
        }
        
        // Load fields
        iter = NULL;
        MonoClassField* field = mono_class_get_fields(_class, &iter);
        while(field != NULL) {
            std::string fieldName = mono_field_full_name(field);
            fieldName = fieldName.substr(fieldName.find(":") + 1);
            
            MonoFieldDesc* fd = new MonoFieldDesc();
            fd->name = fieldName;
            fd->field = field;
            cl->fields[fd->name] = fd;
            
            field = mono_class_get_fields(_class, &iter);
        }
    }
    
    // Process inheritance
    auto cli = classNames.begin();
    for(; cli != classNames.end(); cli++) {
        // Get mono class
        MonoClassDesc* cl = m_classes[*cli];
        Meta::Class* mcl = metaSystem->FindClass(*cli);
        MonoClass* _parent = mono_class_get_parent(cl->_class);
        while(_parent) {
            // Add to inheritance list
            std::string parentName = mono_class_get_name(_parent);
            if(parentName.compare("Object") == 0) { _parent = 0; continue; }
            
            if(mcl) {
                auto ii = mcl->inheritsFrom.find(parentName);
                if(ii == mcl->inheritsFrom.end()) {
                    mcl->inheritsFrom.push_back(parentName);
                }
            }
            
            // Add functions
            auto it = m_classes.find(parentName);
            if(it == m_classes.end()) { _parent = 0; continue; }
            
            MonoClassDesc* desc = m_classes[parentName];
            
            if(desc == 0) {
                _parent = 0;
                continue;
            }
            
            auto fi = desc->methods.begin();
            for(; fi != desc->methods.end(); fi++) {
                // Make sure we don't override an override
                if(cl->methods.find(fi->first) != cl->methods.end()) continue;
                if(fi->first.length() > 8) {
                    if(fi->first.substr(0, 8) == "internal") continue;
                }
                
                cl->methods[fi->first] = fi->second;
            }
            
            _parent = mono_class_get_parent(_parent);
        }
    }
    
    // Run load function if it exists
    if(loadLibrary != 0) {
        MonoObject* exc = 0;
        mono_runtime_invoke(loadLibrary, nullptr, nullptr, &exc);
        if(exc) {
            mono_print_unhandled_exception(exc);
            GIGA_ASSERT(false, "Unable to run LoadLibrary function.");
        }
    }
    
    return(image);
}

void ScriptingSystem::Update(float delta) {
    PROFILE_START_AREA("ScriptingSystem");
    Variant* d = new Variant(delta);
    
    // Add to any existing script components
    World* world = World::GetInstance();
    Array<ScriptComponent*> components = world->FindComponents<ScriptComponent>();
    auto i = components.begin();
    for (; i != components.end(); i++) {
        if((*i)->className.length() == 0) continue;
        
        // Get the class name
        auto ci = m_classes.find((*i)->className);
        GIGA_ASSERT(ci != m_classes.end(), "Class name not found.");
        
        // Get the update method
        auto mi = ci->second->methods.find("Update");
        if(mi == ci->second->methods.end()) {
            // Does not implement update function
            continue;
        }
        
        // If we cannot find a cached object, create one
        MonoObject* remote = this->GetRemoteObject((*i)->GetParent());
        GIGA_ASSERT(remote != 0, "Cannot find or create object.");
        
        void* args[1];
        double ddelta = delta;
        args[0] = &ddelta;
        MonoObject* exc = 0;
        mono_runtime_invoke(mi->second->method, remote, args, &exc);
        if(exc) {
            mono_print_unhandled_exception(exc);
            GIGA_ASSERT(false, "Unable to run Update function.");
        }
    }
    
    delete d;

    PROFILE_END_AREA("ScriptingSystem");
}

void ScriptingSystem::RegisterEventHandler(std::string type, std::string funcName) {
    // Get current thread out of isolate
    ScriptComponent* component = 0;
    
    // Get the registered type ID
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    uint32_t typeID = messageSystem->GetMessageType(type);
    
    EventHandler* eh = new EventHandler;
    eh->type = typeID;
    eh->funcName = funcName;
    eh->component = component;
    
    m_eventHandlers.push_back(eh);
    
    // Register with message system
    messageSystem->RegisterCallback(this, type, ScriptEventHandler);
}

void ScriptingSystem::ScriptEventHandler(GigaObject* obj, Message* message) {
    /*Variant* mv = new Variant(message);
    
    ScriptingSystem* scriptingSystem = (ScriptingSystem*)obj;
    
    Array<EventHandler*>::iterator it = scriptingSystem->m_eventHandlers.begin();
    for(; it != scriptingSystem->m_eventHandlers.end(); it++) {
        if((*it)->type == message->type) {
            Entity* ent = (*it)->component->GetParent();
            Variant* parent = 0;
            if(ent) {
                parent = new Variant(ent);
                (*it)->component->SetGlobal("GameObject", parent);
            }
            
            (*it)->component->CallFunction((*it)->funcName, 1, &mv);
            
            if(parent) {
                delete parent;
            }
        }
    }
    
    delete mv;*/
}

MonoObject* ScriptingSystem::GetRemoteObject(GigaObject* obj) {
    auto it = m_objects.begin();
    for(; it != m_objects.end(); it++) {
        if((*it)->local == obj) {
            return((*it)->remote);
        }
    }
    
    // If we get here, we need to create a new object
    auto ci = m_classes.find(obj->GetGigaName());
    
    // If we have an entity with a ScriptComponent attached, make sure we initialize that type
    Entity* entity = dynamic_cast<Entity*>(obj);
    if(entity) {
        ScriptComponent* script = entity->GetComponent<ScriptComponent>();
        if(script) {
            ci = m_classes.find(script->className);
        }
    }
    
    GIGA_ASSERT(ci != m_classes.end(), "Class name not loaded.");
    
    // Create object
    MonoObject* mobj = mono_object_new(mono_domain_get(), ci->second->_class);
    
    // Cache object so it doesn't get re-created in constructor
    ScriptObject* cobj = new ScriptObject();
    cobj->local = obj;
    cobj->remote = mobj;
    cobj->classType = ci->second;
    
    m_objects.push_back(cobj);
    
    // Run constructor
    mono_runtime_object_init(mobj);
    //mono_runtime_invoke(ci->second->methods[".ctor"]->method, mobj, nullptr, nullptr);
    
    return(mobj);
}

GigaObject* ScriptingSystem::GetLocalObject(MonoObject* obj) {
    auto it = m_objects.begin();
    for(; it != m_objects.end(); it++) {
        if((*it)->remote == obj) {
            return((*it)->local);
        }
    }
    
    // If we get here, we need to create a new local object
    MonoClass* _class = mono_object_get_class(obj);
    std::string className = mono_class_get_name(_class);
    
    // Find class in meta system
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Meta::Class* cl = metaSystem->FindClass(className);
    GIGA_ASSERT(cl != 0, "Class type not found.");
    
    // Find class in local system
    auto ci = m_classes.find(className);
    GIGA_ASSERT(ci != m_classes.end(), "Remote class type not found.");
    
    // If this is a singleton, get the singleton
    if(cl->singleton == true) {
        GigaObject* si = metaSystem->GetSingleton(className);
        GIGA_ASSERT(si != 0, "Singleton not found.");
        
        return(si);
    }
    
    // Process inheritance to find top-level native C++ class
    Meta::Class* mc = metaSystem->FindClass(cl->name);
    if(mc->native == false) {
        auto ci = mc->inheritsFrom.begin();
        for(; ci != mc->inheritsFrom.end(); ci++) {
            Meta::Class* mci = metaSystem->FindClass(*ci);
            if(mci) {
                if(mci->native == true) {
                    mc = mci;
                }
            }
        }
        
        if(ci == mc->inheritsFrom.end()) mc = metaSystem->FindClass("GigaObject");
    }
    
    // Otherwise, create a new object
    GigaObject* newobj = mc->CreateObject();
    
    // Cache
    ScriptObject* cobj = new ScriptObject();
    cobj->local = newobj;
    cobj->remote = obj;
    cobj->classType = ci->second;
    
    m_objects.push_back(cobj);
    
    return(newobj);
}

Variant* ScriptingSystem::MonoObjectToVariant(MonoObject* mobj) {
    if(mobj == 0) return(new Variant(0));
    
    MonoClass* _class = mono_object_get_class(mobj);
    std::string className = mono_class_get_name(_class);
    
    MonoClassDesc* cl = m_classes[className];
    
    int variantType = m_variantMappings[className];
    
    if(_class == m_cachedTypes[Variant::VAR_INT32]) {
        int32_t* iv = (int32_t*)mono_object_unbox(mobj);
        return(new Variant(*iv));
    }
    
    if(_class == m_cachedTypes[Variant::VAR_UINT32]) {
        uint32_t* iv = (uint32_t*)mono_object_unbox(mobj);
        return(new Variant(*iv));
    }
    
    if(_class == m_cachedTypes[Variant::VAR_INT64]) {
        int64_t* iv = (int64_t*)mono_object_unbox(mobj);
        return(new Variant(*iv));
    }
    
    if(_class == m_cachedTypes[Variant::VAR_UINT64]) {
        uint64_t* iv = (uint64_t*)mono_object_unbox(mobj);
        return(new Variant(*iv));
    }
    
    if(_class == m_cachedTypes[Variant::VAR_BOOL]) {
        bool* bv = (bool*)mono_object_unbox(mobj);
        return(new Variant(*bv));
    }
    
    if(_class == m_cachedTypes[Variant::VAR_STRING]) {
        MonoString* str = (MonoString*)mobj;
        char* val = mono_string_to_utf8(str);
        return(new Variant(val));
    }
    
    if(_class == m_cachedTypes[Variant::VAR_FLOAT]) {
        double* iv = (double*)mono_object_unbox(mobj);
        float f = *iv;
        return(new Variant(f));
    }
    
    if(variantType == Variant::VAR_VECTOR2) {
        double x, y;
        mono_field_get_value(mobj, cl->fields["x"]->field, &x);
        mono_field_get_value(mobj, cl->fields["y"]->field, &y);
        
        return(new Variant(vector2(x, y)));
    }
    
    if(variantType == Variant::VAR_VECTOR3) {
        double x, y, z;
        mono_field_get_value(mobj, cl->fields["x"]->field, &x);
        mono_field_get_value(mobj, cl->fields["y"]->field, &y);
        mono_field_get_value(mobj, cl->fields["z"]->field, &z);
        
        return(new Variant(vector3(x, y, z)));
    }
    
    if(variantType == Variant::VAR_VECTOR4) {
        double x, y, z, w;
        mono_field_get_value(mobj, cl->fields["x"]->field, &x);
        mono_field_get_value(mobj, cl->fields["y"]->field, &y);
        mono_field_get_value(mobj, cl->fields["z"]->field, &z);
        mono_field_get_value(mobj, cl->fields["w"]->field, &w);
        
        return(new Variant(vector4(x, y, z, w)));
    }
    
    if(variantType == Variant::VAR_QUATERNION) {
        double x, y, z, w;
        mono_field_get_value(mobj, cl->fields["x"]->field, &x);
        mono_field_get_value(mobj, cl->fields["y"]->field, &y);
        mono_field_get_value(mobj, cl->fields["z"]->field, &z);
        mono_field_get_value(mobj, cl->fields["w"]->field, &w);
        
        return(new Variant(quaternion(w, x, y, z)));
    }
    
    if(cl->_class == m_cachedTypes[Variant::VAR_OBJECT]) {
        GigaObject* gobj = this->GetLocalObject(mobj);
        return(new Variant(gobj));
    }
    
    return(0);
}

MonoObject* ScriptingSystem::VariantToMonoObject(Variant* var, std::string classHint) {
    MonoObject* mobj = 0;
    
    if(var->IsInt()) {
        MonoClass* cl = m_cachedTypes[Variant::VAR_INT32];
        int iv = var->AsInt();
        mobj = mono_value_box(mono_domain_get(), cl, &iv);
    }
    
    if(var->IsBool()) {
        MonoClass* cl = m_cachedTypes[Variant::VAR_BOOL];
        bool bv = var->AsBool();
        mobj = mono_value_box(mono_domain_get(), cl, &bv);
    }
    
    if(var->IsUInt()) {
        MonoClass* cl = m_cachedTypes[Variant::VAR_UINT32];
        unsigned int iv = var->AsUInt();
        mobj = mono_value_box(mono_domain_get(), cl, &iv);
    }
    
    if(var->IsFloat()) {
        MonoClass* cl = m_cachedTypes[Variant::VAR_FLOAT];
        double dv = var->AsFloat();
        mobj = mono_value_box(mono_domain_get(), cl, &dv);
    }
    
    if(var->IsString()) {
        mobj = (MonoObject*)mono_string_new(mono_domain_get(), var->AsString().c_str());
    }
    
    if(var->IsObject()) {
        mobj = this->GetRemoteObject(var->AsObject());
    }
    
    if(var->IsArray()) {
        // Special handling for empty arrays
        int size = var->Size();
        if(size == 0) {
            MonoClass* cl = m_classes[classHint]->_class;
            MonoArray* array = mono_array_new(mono_domain_get(), cl, size);
            return((MonoObject*)array);
        }
        
        // Otherwise, base type on first item
        Variant* v = var->At(0);
        if(v->IsInt()) {
            MonoClass* cl = m_cachedTypes[Variant::VAR_INT32];
            MonoArray* array = mono_array_new(mono_domain_get(), cl, size);
            for(int i = 0; i < size; i++) {
                v = var->At(i);
                int iv = v->AsInt();
                MonoObject* mobj = mono_value_box(mono_domain_get(), cl, &iv);
                mono_array_setref(array, i, mobj);
            }
            return((MonoObject*)array);
        }
        
        if(v->IsUInt()) {
            MonoClass* cl = m_cachedTypes[Variant::VAR_UINT32];
            MonoArray* array = mono_array_new(mono_domain_get(), cl, size);
            for(int i = 0; i < size; i++) {
                v = var->At(i);
                int iv = v->AsUInt();
                MonoObject* mobj = mono_value_box(mono_domain_get(), cl, &iv);
                mono_array_setref(array, i, mobj);
            }
            return((MonoObject*)array);
        }
        
        if(v->IsFloat()) {
            MonoClass* cl = m_cachedTypes[Variant::VAR_FLOAT];
            MonoArray* array = mono_array_new(mono_domain_get(), cl, size);
            for(int i = 0; i < size; i++) {
                v = var->At(i);
                float f = v->AsFloat();
                MonoObject* mobj = mono_value_box(mono_domain_get(), cl, &f);
                mono_array_setref(array, i, mobj);
            }
            return((MonoObject*)array);
        }
        
        if(v->IsString()) {
            MonoClass* cl = m_cachedTypes[Variant::VAR_STRING];
            MonoArray* array = mono_array_new(mono_domain_get(), cl, size);
            for(int i = 0; i < size; i++) {
                v = var->At(i);
                MonoObject* mobj = (MonoObject*)mono_string_new(mono_domain_get(), v->AsString().c_str());
                mono_array_setref(array, i, mobj);
            }
            return((MonoObject*)array);
        }
        
        if(v->IsObject()) {
            MonoClass* cl = m_classes[classHint]->_class;
            MonoArray* array = mono_array_new(mono_domain_get(), cl, size);
            for(int i = 0; i < size; i++) {
                v = var->At(i);
                MonoObject* mobj = this->GetRemoteObject(v->AsObject());
                mono_array_setref(array, i, mobj);
            }
            return((MonoObject*)array);
        }
    }
    
    if(var->IsVector2()) {
        MonoClassDesc* cl = m_classes["Vector2"];
        mobj = mono_object_new(mono_domain_get(), cl->_class);
        mono_runtime_object_init(mobj);
        vector2 vec = var->AsVector2();
        
        double x = vec.x;
        double y = vec.y;
        mono_field_set_value(mobj, cl->fields["x"]->field, &x);
        mono_field_set_value(mobj, cl->fields["y"]->field, &y);
    }
    
    if(var->IsVector3()) {
        MonoClassDesc* cl = m_classes["Vector3"];
        mobj = mono_object_new(mono_domain_get(), cl->_class);
        mono_runtime_object_init(mobj);
        vector3 vec = var->AsVector3();
        
        double x = vec.x;
        double y = vec.y;
        double z = vec.z;
        mono_field_set_value(mobj, cl->fields["x"]->field, &x);
        mono_field_set_value(mobj, cl->fields["y"]->field, &y);
        mono_field_set_value(mobj, cl->fields["z"]->field, &z);
    }
    
    if(var->IsVector4()) {
        MonoClassDesc* cl = m_classes["Vector4"];
        mobj = mono_object_new(mono_domain_get(), cl->_class);
        mono_runtime_object_init(mobj);
        vector4 vec = var->AsVector4();
        
        double x = vec.x;
        double y = vec.y;
        double z = vec.z;
        double w = vec.w;
        mono_field_set_value(mobj, cl->fields["x"]->field, &x);
        mono_field_set_value(mobj, cl->fields["y"]->field, &y);
        mono_field_set_value(mobj, cl->fields["z"]->field, &z);
        mono_field_set_value(mobj, cl->fields["w"]->field, &w);
    }
    
    if(var->IsQuaternion()) {
        MonoClassDesc* cl = m_classes["Quaternion"];
        mobj = mono_object_new(mono_domain_get(), cl->_class);
        mono_runtime_object_init(mobj);
        quaternion q = var->AsQuaternion();
        
        double x = q.x;
        double y = q.y;
        double z = q.z;
        double w = q.w;
        mono_field_set_value(mobj, cl->fields["x"]->field, &x);
        mono_field_set_value(mobj, cl->fields["y"]->field, &y);
        mono_field_set_value(mobj, cl->fields["z"]->field, &z);
        mono_field_set_value(mobj, cl->fields["w"]->field, &w);
    }
    
    return(mobj);
}

Variant* ScriptingSystem::CallFunction(std::string className, std::string func, int argc, Variant** argv, void* obj) {
    auto ci = m_classes.find(className);
    GIGA_ASSERT(ci != m_classes.end(), "Class name not found.");

    // Get function
    auto fi = ci->second->methods.find(func);
    GIGA_ASSERT(fi != ci->second->methods.end(), "Function not found.");

    // Set params
    void** args = (void**)malloc(sizeof(void*) * argc);
    for (int i = 0; i < argc; i++) {
        int vtype = argv[i]->GetType();
        switch (vtype) {
        case Variant::VAR_STRING:
            args[i] = mono_string_new(mono_domain_get(), argv[i]->AsString().c_str());
            break;
        case Variant::VAR_VECTOR2:
        case Variant::VAR_VECTOR3:
        case Variant::VAR_VECTOR4:
        case Variant::VAR_QUATERNION:
        case Variant::VAR_OBJECT:
        case Variant::VAR_ARRAY:
            args[i] = this->VariantToMonoObject(argv[i]);
            break;
        default:
            args[i] = argv[i]->GetPtr();
            break;
        };
    }

    // Call
    MonoObject* exc = 0;
    MonoObject* retval = mono_runtime_invoke(fi->second->method, obj, args, &exc);
    if (exc) {
        mono_print_unhandled_exception(exc);
        GIGA_ASSERT(false, "Unable to run function.");
    }

    free(args);

    /*

     // You might think you want to use mono_runtime_invoke_array, but it will crash on macOS 10.14+ attempting to
     // get into GC in the mono internals

     MonoArray *params = mono_array_new(mono_domain_get(), mono_get_object_class(), argc);
    for(int i = 0; i < argc; i++) {
        MonoObject *param_obj = this->VariantToMonoObject(argv[i]);
        mono_array_setref(params, i, param_obj);
    }

    // Call
    MonoObject* exc = 0;
    MonoObject* retval = mono_runtime_invoke_array(fi->second->method, mobj, params, &exc);
    if(exc) {
        mono_print_unhandled_exception(exc);
        GIGA_ASSERT(false, "Unable to run function.");
    }*/

    return(MonoObjectToVariant(retval));
}

Variant* ScriptingSystem::CallFunction(GigaObject* obj, std::string func, int argc, Variant** argv) {
    // Get the object
    MonoObject* mobj = this->GetRemoteObject(obj);
    std::string className = obj->GetGigaName();
    
    return(this->CallFunction(className, func, argc, argv, mobj));
}
