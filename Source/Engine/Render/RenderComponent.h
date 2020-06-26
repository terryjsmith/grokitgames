
#ifndef rendercomponent_h
#define rendercomponent_h

#include <Core/TransformableComponent.h>
#include <Render/Renderable.h>
#include <Core/VariantRef.h>
#include <Render/Sphere.h>

class GIGA_API RenderComponent : public TransformableComponent {
public:
    RenderComponent();
    virtual ~RenderComponent();
    
    void SetProperty(std::string property, VariantRef val);
    VariantRef GetProperty(std::string property);
    
    /**
     * Get oriented sphere
     */
    Sphere* GetBoundingSphere(matrix4 parent = matrix4(1.0f));

public:
    // Renderable base object
    Renderable* renderable;
    
    // Children
    Array<RenderComponent*> children;
    
    // Lit or un-lit
    bool applyLighting;
    
    // Additional properties
    std::map<std::string, VariantRef> m_properties;
    
    // Oriented bounding sphere
    Sphere* m_boundingSphere;
};

#endif
