
#include <Core/World.h>
#include <Core/Application.h>
#include <Core/MetaSystem.h>
#include <Core/MessageSystem.h>

World* World::m_instance = 0;

World::World() {
    m_nextEntityID = 1;
}

void World::Initialize() {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    metaSystem->SetSingleton(this);

    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    messageSystem->RegisterMessageType("ENTITY_ADDED");
    messageSystem->RegisterMessageType("ENTITY_REMOVED");
    messageSystem->RegisterMessageType("COMPONENT_ADDED");
    messageSystem->RegisterMessageType("COMPONENT_REMOVED");
}

World* World::GetInstance() {
    if(m_instance == 0) {
        m_instance = new World();
        m_instance->Initialize();
    }
    
    return(m_instance);
}

Entity* World::CreateEntity() {
    Entity* e = new Entity();
    e->entityID = m_nextEntityID++;

    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    messageSystem->Broadcast(new Message("ENTITY_ADDED", e));
    
    m_entities.push_back(e);
    return(e);
}

void World::AddEntity(Entity* entity) {
    if(entity->entityID == 0) {
        entity->entityID = m_nextEntityID++;
    }
    else {
        m_nextEntityID = std::max((uint32_t)m_nextEntityID, entity->entityID);
        m_nextEntityID++;
    }

    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    messageSystem->Broadcast(new Message("ENTITY_ADDED", entity));
    
    m_entities.push_back(entity);
}

Entity* World::FindEntity(int entityID) {
    auto it = m_entities.begin();
    for(; it != m_entities.end(); it++) {
        Entity* e = (*it);
        if(e->entityID == entityID) {
            return(e);
        }
    }
    
    return(0);
}

Entity* World::FindEntity(std::string name) {
    auto it = m_entities.begin();
    for(; it != m_entities.end(); it++) {
        Entity* e = (*it);
        if(e->name == name) {
            return(e);
        }
    }
    
    return(0);
}

void World::Clear() {
    auto it = m_entities.begin();
    for(; it != m_entities.end(); it++) {
        Entity* e = (*it);
        delete e;
    }
    
    m_entities.clear();
}

Array<Component*> World::FindComponents(std::string type) {
    Array<Component*> components;
    auto it = m_entities.begin();
    for(; it != m_entities.end(); it++) {
        Entity* ent = (*it);
        Component* c = ent->GetComponent(type);
        if(c) {
            components.push_back(c);
        }
    }
    
    return(components);
}

void World::RemoveEntity(Entity* entity) {
    auto it = m_entities.find(entity);
    if(it != m_entities.end()) {
        m_entities.erase(it);
    }

    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    messageSystem->Broadcast(new Message("ENTITY_REMOVED", entity));
}
