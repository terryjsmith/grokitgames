
#ifndef pointlightcomponent_h
#define pointlightcomponent_h

#include <Render/LightComponent.h>

#define POINTLIGHT_TEXTURE_SIZE  512

/**
 * An omni-directional light type
 */
GIGA_CLASS()
class GIGA_API PointLightComponent : public LightComponent {
public:
    PointLightComponent() = default;
    ~PointLightComponent() = default;
    
    GIGA_CLASS_NAME("PointLightComponent");
    
    /**
     * Initialize
     */
    void Initialize();
    
    /**
     * Create depth textures
     */
    void GenerateDepthTexture(Scene* scene);
    
    /**
     * Get light matrix
     */
    matrix4 GetLightMatrix();
    
    /**
     * Serialize / deserialize
     */
    virtual void Serialize(DataRecord* record);
    virtual void Deserialize(DataRecord* record);
    
protected:
    // A cached set of "look" vectors for the 6 directions
    struct LookVectors {
        vector3 look;
        vector3 up;
        vector3 right;
    };
    
    // Set look vectors
    LookVectors m_vectors[6];
};

#endif
