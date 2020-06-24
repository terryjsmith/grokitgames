
#ifndef metasystem_h
#define metasystem_h

#include <Core/Meta.h>
#include <Core/System.h>

GIGA_CLASS(Singleton)
class GIGA_API MetaSystem : public System {
public:
    MetaSystem() = default;
    ~MetaSystem() = default;
    
    GIGA_CLASS_NAME("MetaSystem");
    
    /**
     * Initialize (register meta data)
     */
    void Initialize();
    
    /**
     * Find an object definition (by name / type ID)
     */
    Meta::Class* FindClass(std::string name);
    Meta::Class* FindClass(uint32_t typeID);
    
    /**
     * Shortcut to create a class
     */
    GigaObject* CreateClass(std::string name);
    GigaObject* CreateClass(uint32_t typeID);
    
    /**
     * Get registered classes
     */
    Array<Meta::Class*> GetRegisteredClasses() { return m_definitions; }
    
    /**
     * Add class definition
     */
    void AddClass(Meta::Class* cl);
    
    /**
     * Find a singleton instance
     */
    GigaObject* GetSingleton(std::string className);
    
    /**
     * Find a singleton instance
     */
    void SetSingleton(GigaObject* obj);
    
    /**
     * Get next available type ID
     */
    uint32_t GetNextTypeID() { m_maxTypeID += 10; return(m_maxTypeID); }
    
    /**
     * Register a component type
     */
    GIGA_FUNCTION() void RegisterComponentType(std::string type);
    
    /**
     * Get component types
     */
    Array<std::string>& GetComponentTypes() { return m_componentTypes; }
    
protected:
    // Class definitions
    Array<Meta::Class*> m_definitions;
    
    // Singleton instances
    Array<GigaObject*> m_singletons;
    
    // Next type ID
    uint32_t m_maxTypeID;
    
    // Registered component types
    Array<std::string> m_componentTypes;
};

#endif
