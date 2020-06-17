
#include <Core/Entity.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>
#include <Core/DataRecord.h>

Entity::Entity() {
    m_transform = new Transform();
}

Entity::~Entity() {
    if(m_transform) {
        delete m_transform;
        m_transform = 0;
    }
}

Component* Entity::CreateComponent(std::string className) {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Meta::Class* mc = metaSystem->FindClass(className);
    
    Component* component = dynamic_cast<Component*>(mc->CreateObject());
    GIGA_ASSERT(component != 0, "Must be derived from Component class.");
    
    return(component);
}

Component* Entity::CreateComponent(uint32_t typeID) {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Meta::Class* mc = metaSystem->FindClass(typeID);
    
    Component* component = dynamic_cast<Component*>(mc->CreateObject());
    GIGA_ASSERT(component != 0, "Must be derived from Component class.");
    
    return(component);
}

Component* Entity::FindComponent(std::string className) {
    auto it = m_components.begin();
    for(; it != m_components.end(); it++) {
        Component* c = (*it);
        if(c) {
            if(c->GetGigaName() == className) {
                return(c);
            }
        }
    }
    
    return(0);
}

Component* Entity::FindComponent(uint32_t typeID) {
    auto it = m_components.begin();
    for(; it != m_components.end(); it++) {
        Component* c = (*it);
        if(c) {
            Meta::Class* mc = c->GetClass();
            if(mc->typeID == typeID) {
                return(c);
            }
        }
    }
    
    return(0);
}

void Entity::AddComponent(Component* c) {
    auto it = m_components.begin();
    for(; it != m_components.end(); it++) {
        Component* ci = (*it);
        if(c == ci) {
            return;
        }
    }
    
    m_components.push_back(c);
}

void Entity::RemoveComponent(Component* c) {
    auto it = m_components.begin();
    for(; it != m_components.end(); it++) {
        Component* ci = (*it);
        if(c == ci) {
            m_components.erase(it);
            return;
        }
    }
}

void Entity::Serialize(DataRecord* record) {
    entityID = record->Get("entityID")->AsInt();
    name = record->Get("name")->AsString();
}

void Entity::Deserialize(DataRecord* record) {
    record->Set("entityID", new Variant(entityID));
    record->Set("name", new Variant(name));
}
