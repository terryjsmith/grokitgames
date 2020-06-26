
#include <Core/Entity.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>
#include <Core/DataRecord.h>
#include <Core/TransformComponent.h>

Entity::~Entity() {
    auto ci = m_components.begin();
    for(; ci != m_components.end(); ci++) {
        delete(*ci);
    }
    
    m_components.clear();
}

Component* Entity::CreateComponent(std::string className) {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Meta::Class* mc = metaSystem->FindClass(className);
    
    Component* component = dynamic_cast<Component*>(mc->CreateObject());
    GIGA_ASSERT(component != 0, "Must be derived from Component class.");
    
    component->m_parent = this;
    m_components.push_back(component);
    
    // Alert this component
    component->onEntitySet();
    
    // Alert other components
    auto ci = m_components.begin();
    for(; ci != m_components.end(); ci++) {
        (*ci)->onComponentAdded(component);
    }
    
    return(component);
}

Component* Entity::CreateComponent(uint32_t typeID) {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Meta::Class* mc = metaSystem->FindClass(typeID);
    
    Component* component = dynamic_cast<Component*>(mc->CreateObject());
    GIGA_ASSERT(component != 0, "Must be derived from Component class.");
    
    component->m_parent = this;
    m_components.push_back(component);
    
    // Alert this component
    component->onEntitySet();
    
    // Alert other components
    auto ci = m_components.begin();
    for(; ci != m_components.end(); ci++) {
        (*ci)->onComponentAdded(component);
    }
    
    return(component);
}

Component* Entity::GetComponent(std::string className) {
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

Component* Entity::GetComponent(uint32_t typeID) {
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
    
    c->m_parent = this;
    m_components.push_back(c);
    
    // Alert this component
    c->onEntitySet();
    
    // Alert other components
    auto ci = m_components.begin();
    for(; ci != m_components.end(); ci++) {
        (*ci)->onComponentAdded(c);
    }
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
    record->Set("entityID", new Variant(entityID));
    record->Set("name", new Variant(name));
}

void Entity::Deserialize(DataRecord* record) {
    entityID = record->Get("entityID")->AsUInt();
    name = record->Get("name")->AsString();
}
