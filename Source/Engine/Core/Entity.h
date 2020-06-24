
#ifndef entity_h
#define entity_h

#include <Core/Component.h>
#include <Core/Transform.h>

/**
 * Entities - root object made up of components
 */
GIGA_CLASS()
class GIGA_API Entity : public GigaObject {
public:
    Entity();
    ~Entity();
    
    GIGA_CLASS_NAME("Entity");
    
    /**
     * Create component by class
     */
    template<class T>
    T* CreateComponent() {
        T* component = new T();
        Component* c = dynamic_cast<Component*>(component);
        GIGA_ASSERT(c != 0, "Must be derived from Component type.");
        
        c->m_parent = this;
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
    T* GetComponent() {
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
    GIGA_FUNCTION() Component* GetComponent(std::string className);
    
    /**
     * Find component by type ID
     */
    GIGA_FUNCTION() Component* GetComponent(uint32_t typeID);
    
    /**
     * Get all components
     */
    GIGA_FUNCTION() Array<Component*> GetComponents() { return m_components; }

    /**
     * Add a component
     */
    void AddComponent(Component* c);
    
    /**
     * Remove component
     */
    void RemoveComponent(Component* c);
    
    /**
     * Serialize/deserialize entities
     */
    void Serialize(DataRecord* record);
    void Deserialize(DataRecord* record);
    
public:
    // Name
    GIGA_VARIABLE(Get, Set) std::string name;
    
    // ID
    GIGA_VARIABLE(Get, Set) uint32_t entityID;
    
protected:
    // List of components
    Array<Component*> m_components;
};

#endif
