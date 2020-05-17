
#include <Physics/RigidBodyComponent.h>
#include <Core/Entity.h>

RigidBodyComponent::RigidBodyComponent() {
    m_transform = new Transform();
    
    if(m_parent) {
        m_transform->SetParent(m_parent->GetTransform());
    }
}

RigidBodyComponent::~RigidBodyComponent() {
    if(m_transform) {
        delete m_transform;
        m_transform = 0;
    }
}
