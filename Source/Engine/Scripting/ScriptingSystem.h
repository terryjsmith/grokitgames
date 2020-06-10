
#ifndef scriptingsystem_h
#define scriptingsystem_h

#include <Core/System.h>
#include <Scripting/ScriptComponent.h>
#include <Scripting/ScriptVariant.h>
#include <Core/Message.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

GIGA_CLASS(Singleton)
class GIGA_API ScriptingSystem : public System {
public:
    ScriptingSystem() = default;
    ~ScriptingSystem() = default;
    
    GIGA_CLASS_NAME("ScriptingSystem");
    
    void Initialize();
    
    /**
     * Update all script components
     */
    void Update(float delta);
    
    /**
     * Get all registered global variables
     */
    void SetGlobal(std::string name, Variant* value);
    std::map<std::string, ScriptVariant*>& GetGlobals() { return m_globals; }
    
    /**
     * Register event handler
     */
    GIGA_FUNCTION() void RegisterEventHandler(std::string type, std::string funcName);
    
    /**
     * Event handler callback
     */
    static void ScriptEventHandler(GigaObject* obj, Message* message);

protected:
    // Registered globals
    std::map<std::string, ScriptVariant*> m_globals;
    
    // Mono domain
    MonoDomain* m_domain;
    
    // Registered event handlers
    struct EventHandler {
        uint32_t type;
        ScriptComponent* component;
        std::string funcName;
    };
    
    std::vector<EventHandler*> m_eventHandlers;
};

#endif
