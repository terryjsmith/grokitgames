
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

    record->Set("Transform.position", new Variant(m_transform->GetLocalPosition()));
    record->Set("Transform.rotation", new Variant(m_transform->GetLocalRotation()));
    record->Set("Transform.scaling", new Variant(m_transform->GetLocalScaling()));
}

void TransformableComponent::Deserialize(DataRecord* record) {
    Component::Deserialize(record);
    
    m_transform->SetLocalPosition(record->Get("Transform.position")->AsVector3());
    m_transform->SetLocalScaling(record->Get("Transform.scaling")->AsVector3());
    m_transform->SetLocalRotation(record->Get("Transform.rotation")->AsQuaternion());
    this->SetParent();
}
