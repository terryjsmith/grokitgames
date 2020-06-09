
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>
#include <Core/MessageSystem.h>
#include <Core/World.h>
#include <IO/ResourceSystem.h>
#include <IO/Profiler.h>
#include <Core/Directory.h>

void ScriptingSystem::Initialize() {
    m_domain = mono_jit_init("grokitgames");
    
    std::string cwd = Directory::GetCurrent() + std::string("/Resources/");
    std::string libDir = cwd + "Scripting/lib";
    std::string configDir = cwd + "Scripting/etc";
    mono_set_dirs(libDir.c_str(),configDir.c_str());
    
    ScriptThread::Initialize();
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
    
    ScriptThread::Lock(this);
    
    // Update cache
    ScriptThread::UpdateCache();
    
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
    ScriptThread::Unlock();
    PROFILE_END_AREA("ScriptingSystem");
}

void ScriptingSystem::RegisterEventHandler(std::string type, std::string funcName) {
    // Get current thread out of isolate
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    ScriptThread* thread = (ScriptThread*)isolate->GetData(0);
    ScriptComponent* component = thread->GetCurrentScript();
    
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
    scriptingSystem->ScriptThread::Lock(scriptingSystem);
    
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
    
    scriptingSystem->ScriptThread::Unlock();
    delete mv;
}
