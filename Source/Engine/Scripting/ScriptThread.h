
#ifndef scriptthread_h
#define scriptthread_h

#include <Core/GigaObject.h>

class ScriptComponent;

class GIGA_API ScriptThread {
public:
    ScriptThread();
    ~ScriptThread() = default;

    /**
    * Thread initialization
    */
    void Initialize();
    
    /**
     * Load any missing class types
     */
    void UpdateCache();

    /**
    * Thread shutdown
    */
    void Shutdown();

    /**
    * Lock thread
    */
    void Lock(ScriptThread* thread);

    /**
    * Unlock
    */
    void Unlock();

    /**
    * The currently executing script (so events can be proxied back in)
    */
    void SetCurrentScript(ScriptComponent* component) { m_currentScript = component; }
    ScriptComponent* GetCurrentScript() { return m_currentScript; }
    
    /**
     * Create a JS object of a specific class name
     */
    v8::Local<v8::Object> CreateJSObject(std::string className);
    
    /**
     * Check for an existing cached object or create one
     */
    v8::Local<v8::Object> GetJSObject(GigaObject* obj);

    /**
     * A stored function definition
     */
    struct ScriptObjectType {
        // Name
        std::string name;

        // The function template
        v8::Persistent<v8::FunctionTemplate, v8::CopyablePersistentTraits<v8::FunctionTemplate>> functionTemplate;
    };

    /**
     * Get object definitions
     */
    std::vector<ScriptObjectType*> GetScriptObjectTypes();
    
protected:
    // Registered script object types
    std::map<std::string, ScriptObjectType*> m_types;
    
    // Cache of objects
    std::map<GigaObject*, v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>>> m_cached;

    // V8 isolated execution environment (own heap, stack, GC, etc.)
    v8::Isolate* m_isolate;

    // Currently executing script component
    ScriptComponent* m_currentScript;
    
    // Cache of class types
    std::vector<uint32_t> m_classCache;
};

#endif
