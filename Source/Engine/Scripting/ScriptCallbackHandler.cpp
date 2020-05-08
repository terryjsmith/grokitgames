
#include <Scripting/ScriptCallbackHandler.h>
#include <Scripting/ScriptVariant.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>
#include <Scripting/ScriptThread.h>

void ScriptCallbackHandler::New(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    int* existing = (int*)isolate->GetData(1);
    
    if(existing) {
        return info.GetReturnValue().Set(info.This());
    }
    
    v8::HandleScope scope(info.GetIsolate());

    // Convert our callback info into SmartValue objects
    Variant** args = (Variant**)malloc(sizeof(Variant*) * info.Length());
    for (int i = 0; i < info.Length(); i++) {
        args[i] = new ScriptVariant(info[i]);
    }

    v8::Local<v8::Function> func = info.Callee();
    v8::String::Utf8Value funcName(func->GetName());

    // Get the script interface we're dealing with
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    
    Meta::Class* cl = metaSystem->FindClass(*funcName);
    GIGA_ASSERT(cl != 0, "No class found with that type.");

    // Create a new object
    GigaObject* obj = 0;
    if (info.Length() > 0) {
        Meta::Function* fn = cl->FindFunction("New");
        GIGA_ASSERT(fn != 0, "No constructors for object that takes parameters.");

        Variant* newobj = fn->func(0, info.Length(), args);
        GIGA_ASSERT(newobj != 0, "Unable to create new object.");

        obj = newobj->AsObject();
    }
    else {
        obj = cl->CreateObject();
    }

    // Lock mutex on new object
    obj->Lock();

    // Wrap C++ object in JS packaging
    Wrap(obj, info.This());

    for (int i = 0; i < info.Length(); i++) {
        delete args[i];
    }
    free(args);

    // Unlock and return new object
    obj->Unlock();
    return info.GetReturnValue().Set(info.This());
}

void ScriptCallbackHandler::HandleStaticFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
    // Turn our info array into a list of passable Variants
    int argc = info.Length();
    Variant** argv = (Variant**)malloc(sizeof(Variant*) * argc);
    for (int i = 0; i < argc; i++) {
        argv[i] = new ScriptVariant(info[i]);
    }

    // Get current thread out of isolate
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ScriptThread* thread = (ScriptThread*)isolate->GetData(0);
    ScriptComponent* component = thread->GetCurrentScript();

    v8::Local<v8::Object> holder = info.This();
    v8::Local<v8::Function> funcHolder = holder.As<v8::Function>();
    v8::String::Utf8Value thisName(funcHolder->GetName());
    
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    ScriptVariant* retval = 0;
    
    v8::Local<v8::Function> func = info.Callee();
    v8::String::Utf8Value funcName(func->GetName());
    
    GigaObject* singleton = metaSystem->GetSingleton(*thisName);
    if(singleton) {
        singleton->Lock();
        retval = (ScriptVariant*)singleton->Call(*funcName, argc, argv);
        singleton->Unlock();
    }
    else {
        Meta::Class* cl = metaSystem->FindClass(*thisName);
        GIGA_ASSERT(cl != 0, "Class not found.");
        
        Meta::Function* fn = cl->FindFunction(*funcName);
        GIGA_ASSERT(fn != 0, "Function not found.");

        retval = (ScriptVariant*)fn->func(0, argc, argv);
    }
    
    v8::Local<v8::Value> val = retval->GetValue();
    info.GetReturnValue().Set(val);
    
    delete retval;

    // Clean up
    for (int i = 0; i < argc; i++) {
        delete argv[i];
        argv[i] = 0;
    }

    free(argv);
}

void ScriptCallbackHandler::HandleObjectFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info) {
    // Unwrap our object
    GigaObject* jsobj = Unwrap(info.This());
    jsobj->Lock();
    
    // Turn our info array into a list of passable Variants
    int argc = info.Length();
    Variant** argv = (Variant**)malloc(sizeof(Variant*) * argc);
    for (int i = 0; i < argc; i++) {
        argv[i] = new ScriptVariant(info[i]);
    }
    
    // Get current thread out of isolate
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ScriptThread* thread = (ScriptThread*)isolate->GetData(0);
    ScriptComponent* component = thread->GetCurrentScript();
    
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    ScriptVariant* retval = 0;
    
    v8::Local<v8::Function> func = info.Callee();
    v8::String::Utf8Value funcName(func->GetName());
    
    std::string className = jsobj->GetGigaName();
    Meta::Class* cl = metaSystem->FindClass(className);
    GIGA_ASSERT(cl != 0, "Class not found.");
    
    Meta::Function* fn = cl->FindFunction(*funcName);
    GIGA_ASSERT(fn != 0, "Function not found.");

    retval = (ScriptVariant*)fn->func(jsobj, argc, argv);
    
    v8::Local<v8::Value> val = retval->GetValue();
    info.GetReturnValue().Set(val);
    
    delete retval;
    
    // Clean up
    for (int i = 0; i < argc; i++) {
        delete argv[i];
        argv[i] = 0;
    }
    
    free(argv);
    jsobj->Unlock();
}

void ScriptCallbackHandler::HandleObjectGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    // Unwrap our object
    GigaObject* jsobj = Unwrap(info.This());
    jsobj->Lock();
    
    // Get current thread out of isolate
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ScriptThread* thread = (ScriptThread*)isolate->GetData(0);
    ScriptComponent* component = thread->GetCurrentScript();
    
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    ScriptVariant* retval = 0;
    
    v8::String::Utf8Value propName(property);
    Meta::Class* cl = metaSystem->FindClass(jsobj->GetGigaName());
    GIGA_ASSERT(cl != 0, "Class not found.");
    
    Meta::Variable* var = cl->FindVariable(*propName);
    GIGA_ASSERT(var != 0, "Variable not found.");
    
    ScriptVariant* value = (ScriptVariant*)var->getter(jsobj);
    v8::Local<v8::Value> val = value->GetValue();
    info.GetReturnValue().Set(val);
    
    jsobj->Unlock();
}

void ScriptCallbackHandler::HandleObjectSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    // Unwrap our object
    GigaObject* jsobj = Unwrap(info.This());
    jsobj->Lock();
    
    // Get current thread out of isolate
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ScriptThread* thread = (ScriptThread*)isolate->GetData(0);
    ScriptComponent* component = thread->GetCurrentScript();
    
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    ScriptVariant* retval = 0;
    
    v8::String::Utf8Value propName(property);
    Meta::Class* cl = metaSystem->FindClass(jsobj->GetGigaName());
    GIGA_ASSERT(cl != 0, "Class not found.");
    
    Meta::Variable* var = cl->FindVariable(*propName);
    GIGA_ASSERT(var != 0, "Variable not found.");
    
    ScriptVariant* sv = new ScriptVariant(value);
    var->setter(jsobj, sv);
    
    delete sv;
    jsobj->Unlock();
}
