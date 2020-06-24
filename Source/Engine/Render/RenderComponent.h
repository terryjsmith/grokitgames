
#ifndef rendercomponent_h
#define rendercomponent_h

#include <Core/Component.h>
#include <Core/Transform.h>
#include <Render/Renderable.h>
#include <Core/VariantRef.h>
#include <Render/Sphere.h>

class GIGA_API RenderComponent : public Component {
public:
    RenderComponent();
    virtual ~RenderComponent();
    
    void SetProperty(std::string property, VariantRef val);
    VariantRef GetProperty(std::string property);
    
    GIGA_FUNCTION() Transform* GetTransform() { return transform; }
    
    /**
     * Get oriented sphere
     */
    Sphere* GetBoundingSphere(matrix4 parent = matrix4(1.0f));
    
public:
    // Renderable base object
    Renderable* renderable;
    
    // Children
    Array<RenderComponent*> children;
    
    // Transform
    Transform* transform;
    
    // Lit or un-lit
    bool applyLighting;
    
    // Additional properties
    std::map<std::string, VariantRef> m_properties;
    
    // Oriented bounding sphere
    Sphere* m_boundingSphere;
};

#endif
