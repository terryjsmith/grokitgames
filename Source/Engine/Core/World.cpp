
#include <Core/World.h>
#include <Core/Application.h>
#include <Core/MetaSystem.h>

World* World::m_instance = 0;

World::World() {
    m_nextEntityID = 1;
}

void World::Initialize() {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    metaSystem->SetSingleton(this);
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
    
    m_entities.push_back(e);
    return(e);
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
