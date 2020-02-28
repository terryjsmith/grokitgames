
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
    
protected:
    // Class definitions
    std::vector<Meta::Class*> m_definitions;
    
    // Singleton instances
    std::vector<GigaObject*> m_singletons;
};

#endif
