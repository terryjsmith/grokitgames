
#include <Render/RenderComponent.h>
#include <Core/Entity.h>

RenderComponent::RenderComponent() {
    transform = new Transform();
    
    if(m_parent != 0) {
        transform->SetParent(m_parent->GetTransform());
    }
    
    renderable = 0;
    m_boundingSphere = 0;
}

RenderComponent::~RenderComponent() {
    if(transform) {
        delete transform;
        transform = 0;
    }
    
    if(m_boundingSphere) {
        delete m_boundingSphere;
        m_boundingSphere = 0;
    }
}

void RenderComponent::SetProperty(std::string property, VariantRef val) {
    m_properties[property] = val;
}

VariantRef RenderComponent::GetProperty(std::string property) {
    return(m_properties[property]);
}

Sphere* RenderComponent::GetBoundingSphere(matrix4 parent) {
    if(m_boundingSphere == 0) {
        m_boundingSphere = new Sphere();
    }
    
    matrix4 modelMatrix = parent * transform->GetMatrix();
    
    // Get the center of the AABB
    vector3 center;
    for(int i = 0; i < 8; i++) {
        center += renderable->aabb->points[i];
    }
    center /= 8.0f;
    
    // Translate to model matrix
    center = vector3(modelMatrix * vector4(center, 1.0f));
    
    vector3 aabbMin = vector3(modelMatrix * vector4(renderable->aabb->min, 1.0f));
    vector3 aabbMax = vector3(modelMatrix * vector4(renderable->aabb->max, 1.0f));
    float radius = glm::distance(aabbMin, aabbMax) / 2.0f;
    
    m_boundingSphere->Create(center, radius);
    return(m_boundingSphere);
}
