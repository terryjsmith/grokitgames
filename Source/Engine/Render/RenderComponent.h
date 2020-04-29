
#ifndef rendercomponent_h
#define rendercomponent_h

#include <Core/Component.h>
#include <Core/Transform.h>
#include <Render/Renderable.h>
#include <Core/VariantRef.h>

class GIGA_API RenderComponent : public Component {
public:
    RenderComponent();
    virtual ~RenderComponent();
    
    void SetProperty(std::string property, VariantRef val);
    VariantRef GetProperty(std::string property);
    
public:
    // Renderable base object
    Renderable* renderable;
    
    // Children
    std::vector<RenderComponent*> children;
    
    // Transform
    Transform* transform;
    
    // Lit or un-lit
    bool applyLighting;
    
    // Additional properties
    std::map<std::string, VariantRef> m_properties;
};

#endif
