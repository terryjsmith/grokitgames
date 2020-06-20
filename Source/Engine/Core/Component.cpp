
#include <Core/Component.h>
#include <Core/DataRecord.h>
#include <Core/Entity.h>
#include <Core/World.h>

Component::Component() {
    m_parent = 0;
}

void Component::Serialize(DataRecord* record) {
    uint32_t entityID = 0;
    if(m_parent) {
        entityID = m_parent->entityID;
    }
    
    record->Set("entityID", new Variant((uint32_t)entityID));
}

void Component::Deserialize(DataRecord* record) {
    uint32_t entityID = record->Get("entityID")->AsUInt();
    if(entityID) {
        Entity* ent = World::GetInstance()->FindEntity(entityID);
        
        if(ent) ent->AddComponent(this);
    }
}
