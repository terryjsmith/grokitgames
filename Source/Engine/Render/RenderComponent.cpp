
#include <Render/RenderComponent.h>
#include <Core/Entity.h>

RenderComponent::RenderComponent() {
    transform = new Transform();
    
    if(m_parent != 0) {
        transform->SetParent(m_parent->GetTransform());
    }
    
    renderable = 0;
}

RenderComponent::~RenderComponent() {
    if(transform) {
        delete transform;
        transform = 0;
    }
}

void RenderComponent::SetProperty(std::string property, VariantRef val) {
    m_properties[property] = val;
}

VariantRef RenderComponent::GetProperty(std::string property) {
    return(m_properties[property]);
}
