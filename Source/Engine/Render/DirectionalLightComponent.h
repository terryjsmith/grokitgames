
#ifndef directionallightcomponent_h
#define directionallightcomponent_h

#include <Render/LightComponent.h>

/**
 * Our implementation of a directional light with cascaded shadow maps
 */
GIGA_CLASS()
class GIGA_API DirectionalLightComponent : public LightComponent {
public:
    DirectionalLightComponent() = default;
    ~DirectionalLightComponent() = default;
    
    GIGA_CLASS_NAME("DirectionalLightComponent");
    
    /**
     * Initialize
     */
    void Initialize();
    
    /**
     * Generate depth texture
     */
    void GenerateDepthTexture(Scene* scene);
    
    /**
     * Serialize / deserialize
     */
    virtual void Serialize(DataRecord* record);
    virtual void Deserialize(DataRecord* record);
};

#endif
