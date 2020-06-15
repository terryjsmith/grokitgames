
#ifndef scriptingsystem_h
#define scriptingsystem_h

#include <Core/System.h>
#include <Scripting/ScriptComponent.h>
#include <Core/Message.h>
#include <Scripting/MonoObjects.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

void RegisterMonoFunctions();

GIGA_CLASS(Singleton)
class GIGA_API ScriptingSystem : public System {
public:
    ScriptingSystem() = default;
    ~ScriptingSystem() = default;
    
    GIGA_CLASS_NAME("ScriptingSystem");
    
    void Initialize();
    
    /**
     * Run game initialization (after all systems are initialized)
     */
    void Start();
    
    /**
     * Update all script components
     */
    void Update(float delta);
    
    /**
     * Load an additional assembly (classes/data/code)
     */
    MonoImage* LoadLibrary(std::string filename);
    
    /**
     * Register event handler
     */
    GIGA_FUNCTION() void RegisterEventHandler(std::string type, std::string funcName);
    
    /**
     * Event handler callback
     */
    static void ScriptEventHandler(GigaObject* obj, Message* message);
    
    /**
     * Local object cache
     */
    MonoObject* GetRemoteObject(GigaObject* obj);
    GigaObject* GetLocalObject(MonoObject* obj);
    
    /**
     * Variant to MonoObject and vice versa
     */
    Variant* MonoObjectToVariant(MonoObject* mobj);
    MonoObject* VariantToMonoObject(Variant* var);
    
    /**
     * Constructor for mapping
     */
    static GigaObject* internal_GigaObject_Ctor(MonoObject* obj);
    
    /**
     * Call a function on a script object
     */
    Variant* CallFunction(GigaObject* obj, std::string func, int argc, Variant** argv);

protected:
    // Object cache
    struct CachedObject {
        GigaObject* local;
        MonoObject* remote;
    };
    
    std::vector<CachedObject*> m_objects;
    
    // Cache of classes
    std::map<std::string, MonoClassDesc*> m_classes;
    
    // Mono domain
    MonoDomain* m_domain;
    
    // Registered event handlers
    struct EventHandler {
        uint32_t type;
        ScriptComponent* component;
        std::string funcName;
    };
    
    std::vector<EventHandler*> m_eventHandlers;
    
    // Mappings from Mono types to variant types
    std::map<std::string, uint32_t> m_variantMappings;
    
    // Cached class types for basic MonoObject classes (int, bool, double, etc.)
    std::map<uint32_t, MonoClass*> m_cachedTypes;
};

#endif
