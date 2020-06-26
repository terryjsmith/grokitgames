
#include <Core/TransformableComponent.h>
#include <Core/TransformComponent.h>
#include <Core/Entity.h>
#include <Core/DataRecord.h>

TransformableComponent::TransformableComponent() {
    m_transform = new Transform();
}

TransformableComponent::~TransformableComponent() {
    if(m_transform) {
        delete m_transform;
        m_transform = 0;
    }
}

void TransformableComponent::onComponentAdded(Component* c) {
    TransformComponent* tc = dynamic_cast<TransformComponent*>(c);
    
    if(tc != 0 && tc != this) {
        m_transform->SetParent(tc->GetTransform());
    }
}

void TransformableComponent::onEntitySet() {
    this->SetParent();
}

void TransformableComponent::SetParent() {
    if(m_parent != 0) {
        TransformComponent* tc = m_parent->GetComponent<TransformComponent>();
        if(tc != this)
            m_transform->SetParent(tc->GetTransform());
    }
}

void TransformableComponent::Serialize(DataRecord* record) {
    Component::Serialize(record);

    record->Set("transform", new Variant(m_transform));
}

void TransformableComponent::Deserialize(DataRecord* record) {
    Component::Deserialize(record);
    
    Transform* transform = record->Get("transform")->AsObject<Transform*>();
    if(m_transform && m_transform != transform) {
        delete m_transform;
    }
    
    m_transform = transform;
    this->SetParent();
}
