
#ifndef world_h
#define world_h

#include <Core/Entity.h>

/**
 * Our World contains all Entity objects
 */
GIGA_CLASS(Singleton)
class GIGA_API World : public GigaObject {
public:
    World();
    ~World() = default;
    
    GIGA_CLASS_NAME("World");
    
    /**
     * Initialize the whole world
     */
    void Initialize();
    
    /**
     * Get the instance of this object
     */
    static World* GetInstance();
    
    /**
     * Create a new entity in the world
     */
    GIGA_FUNCTION() Entity* CreateEntity();
    
    /**
     * Add a new entity to the world
     */
    GIGA_FUNCTION() void AddEntity(Entity* entity);
    
    /**
     * Find an entity by ID
     */
    GIGA_FUNCTION() Entity* FindEntity(int entityID);
    
    /**
     * Find entity by name
     */
    GIGA_FUNCTION() Entity* FindEntity(std::string name);
    
    /**
     * Remove entity
     */
    GIGA_FUNCTION() void RemoveEntity(Entity* entity);
    
    /**
     * Clear the world (delete all entities)
     */
    void Clear();
    
    /**
     * Find all components by type
     */
    template<class T> Array<T*> FindComponents() {
        Array<T*> components;
        auto it = m_entities.begin();
        for(; it != m_entities.end(); it++) {
            Entity* ent = (*it);
            Array<T*> cs = ent->GetComponents<T>();
            auto it = cs.begin();
            for(; it != cs.end(); it++) {
                components.push_back(*it);
            }
        }
        
        return(components);
    }
    
    /**
     * Find all components by type name (will not do intermediate types)
     */
    Array<Component*> FindComponents(std::string type);
    
    /**
     * Get next available entity ID
     */
    GIGA_FUNCTION() uint32_t GetNextEntityID() { return m_nextEntityID++; }
    
    /**
     * Get all entities
     */
    Array<Entity*> GetEntities() { return m_entities; }
    
protected:
    // All entities in the world
    Array<Entity*> m_entities;
    
    // Singleton
    static World* m_instance;
    
    // Next entity ID
    int m_nextEntityID;
};

#endif
