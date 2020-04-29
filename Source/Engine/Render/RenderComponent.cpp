
#include <Render/RenderComponent.h>

RenderComponent::RenderComponent() {
    transform = new Transform();
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
