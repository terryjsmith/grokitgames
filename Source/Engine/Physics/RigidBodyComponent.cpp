
#include <Physics/RigidBodyComponent.h>
#include <Core/TransformComponent.h>
#include <Core/Entity.h>

RigidBodyComponent::RigidBodyComponent() {
    m_transform = new Transform();
    
    if(m_parent != 0) {
        TransformComponent* tc = m_parent->GetComponent<TransformComponent>();
        m_transform->SetParent(tc->GetTransform());
    }
}

RigidBodyComponent::~RigidBodyComponent() {
    if(m_transform) {
        delete m_transform;
        m_transform = 0;
    }
}
