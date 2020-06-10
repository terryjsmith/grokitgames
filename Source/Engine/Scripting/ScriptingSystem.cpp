
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>
#include <Core/MessageSystem.h>
#include <Core/World.h>
#include <IO/ResourceSystem.h>
#include <IO/Profiler.h>
#include <Core/Directory.h>

class MeshComponent {

};

MonoClass* mcClass = 0;

void StartAnimation(MeshComponent* mc, MonoString* animation, bool loop, float weight) {
    int a = 0;
}

MonoObject* LoadMeshComponent(MonoString* filename) {
    char* fname = mono_string_to_utf8(filename);
    
    MeshComponent* mc = new MeshComponent();
    return(mono_value_box(mono_domain_get(), mcClass, (void*)mc));
}

void ScriptingSystem::Initialize() {
    std::string cwd = "C:\\Users\\Smith\\Documents\\Projects\\ThirdParty\\mono\\";
    std::string libDir = cwd + "lib";
    std::string configDir = cwd + "etc";
    mono_set_dirs(libDir.c_str(), configDir.c_str());

    m_domain = mono_jit_init("GrokItGames");

    MonoAssembly* assembly = mono_domain_assembly_open(m_domain, "Resources\\Scripts\\ClassLibrary1.dll");
    mono_add_internal_call("GIGA.AnimationSystem::StartAnimation", StartAnimation);
    mono_add_internal_call("GIGA.MeshComponent::Create", LoadMeshComponent);

    MonoImage* image = mono_assembly_get_image(assembly);
    MonoClass* main_class = mono_class_from_name(image, "GIGA", "Engine");
    mcClass = mono_class_from_name(image, "GIGA", "MeshComponent");

    MonoMethodDesc* entry_point_method_desc = mono_method_desc_new("GIGA.Engine:Startup()", true);
    MonoMethod* entry_point_method = mono_method_desc_search_in_class(entry_point_method_desc, main_class);

    mono_runtime_invoke(entry_point_method, nullptr, nullptr, nullptr);
}

void ScriptingSystem::SetGlobal(std::string name, Variant* value) {
    m_globals[name] = (ScriptVariant*)value;
    
    // Add to any existing script components
    World* world = World::GetInstance();
    std::vector<ScriptComponent*> components = world->FindComponents<ScriptComponent>();
    std::vector<ScriptComponent*>::iterator i = components.begin();
    for (i; i != components.end(); i++) {
        (*i)->SetGlobal(name, value);
    }
}

void ScriptingSystem::Update(float delta) {
    PROFILE_START_AREA("ScriptingSystem");
    Variant* d = new Variant(delta);
    
    // Add to any existing script components
    World* world = World::GetInstance();
    std::vector<ScriptComponent*> components = world->FindComponents<ScriptComponent>();
    std::vector<ScriptComponent*>::iterator i = components.begin();
    for (; i != components.end(); i++) {
        Entity* parent = (*i)->GetParent();
        Variant* p = new Variant(parent);
        if(parent) {
            (*i)->SetGlobal("GameObject", p);
        }
        
        (*i)->CallFunction("Update", 1, &d);
        
        delete p;
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
    Variant* mv = new Variant(message);
    
    ScriptingSystem* scriptingSystem = (ScriptingSystem*)obj;
    
    std::vector<EventHandler*>::iterator it = scriptingSystem->m_eventHandlers.begin();
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
    
    delete mv;
}
