
#include <Core/TransformComponent.h>
#include <Core/DataRecord.h>

TransformComponent::TransformComponent() {
    m_transform = new Transform();
}

TransformComponent::~TransformComponent() {
    if(m_transform) {
        delete m_transform;
    }
}

void TransformComponent::Serialize(DataRecord* record) {
    Component::Serialize(record);
    record->Set("transform", new Variant(m_transform));
}

void TransformComponent::Deserialize(DataRecord* record) {
    Component::Deserialize(record);
    m_transform = record->Get("transform")->AsObject<Transform*>();
}
