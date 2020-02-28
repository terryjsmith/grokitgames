
#ifndef entity_h
#define entity_h

#include <Core/Component.h>

/**
 * Entities - root object made up of components
 */
GIGA_CLASS()
class GIGA_API Entity : public GigaObject {
public:
    Entity() = default;
    ~Entity() = default;
    
    GIGA_CLASS_NAME("Entity");
    
    /**
     * Create component by class
     */
    template<class T>
    T* CreateComponent() {
        T* component = new T();
        GIGA_ASSERT(dynamic_cast<Component*>(component) != 0, "Must be derived from Component type.");
        
        m_components.push_back(component);
        return(component);
    }
    
    /**
     * Create component from string class name
     */
    GIGA_FUNCTION() Component* CreateComponent(std::string className);
    
    /**
     * Create component from class type ID
     */
    GIGA_FUNCTION() Component* CreateComponent(uint32_t typeID);
    
    /**
     * Find component by class type
     */
    template<class T>
    T* FindComponent() {
        auto it = m_components.begin();
        for(; it != m_components.end(); it++) {
            T* obj = dynamic_cast<T*>(*it);
            if(obj != 0) {
                return(obj);
            }
        }
        
        return(0);
    }
    
    /**
     * Find component by class name
     */
    GIGA_FUNCTION() Component* FindComponent(std::string className);
    
    /**
     * Find component by type ID
     */
    GIGA_FUNCTION() Component* FindComponent(uint32_t typeID);
    
    /**
     * Add a component
     */
    void AddComponent(Component* c);
    
    /**
     * Remove component
     */
    void RemoveComponent(Component* c);
    
public:
    // Name
    std::string name;
    
    // ID
    uint32_t entityID;
    
protected:
    // List of components
    std::vector<Component*> m_components;
};

#endif
