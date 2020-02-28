
#ifndef lightcomponent_h
#define lightcomponent_h

#include <Render/RenderComponent.h>
#include <Render/CameraComponent.h>
#include <Render/Framebuffer.h>
#include <Render/Texture.h>
#include <Render/DepthPass.h>

class Scene;

/**
 * Base light class
 */
class GIGA_API LightComponent : public RenderComponent {
public:
    ~LightComponent();
    
    /**
     * Light types
     */
    enum {
        LIGHT_DIRECTIONAL = 1,
        LIGHT_POINT,
        LIGHT_SPOT
    };
    
    /**
     * Initialize
     */
    virtual void Initialize() = 0;
    
    /**
     * Get light type
     */
    GIGA_FUNCTION() int GetType() { return m_type; }
    
    /**
     * Get/set color
     */
    GIGA_FUNCTION() void SetColor(vector3 color) { m_color = color; }
    GIGA_FUNCTION() vector3 GetColor() { return m_color; }
    
    /**
     * Get/set distance
     */
    GIGA_FUNCTION() void SetAttenuation(float distance) { m_attenuation = distance; }
    GIGA_FUNCTION() float GetAttenuation() { return m_attenuation; }
    
    /**
     * Generate depth texture
     */
    virtual void GenerateDepthTexture(Scene* scene) = 0;
    
    /**
     * Get depth texture
     */
    Texture* GetDepthTexture(int pass) { return m_depthTextures[pass]; }
    
    /**
     * Get attached "camera"
     */
    CameraComponent* GetCamera(int pass) { return m_cameras[pass]; }
    
    /**
     * Get light matrix
     */
    virtual matrix4 GetLightMatrix() { return matrix4(1.0); }
    
protected:
    // Create subclass types (spotlight, directional light, etc.)
    LightComponent();
    
protected:
    // The color of light
    vector3 m_color;
    
    // Light type
    unsigned int m_type;
    
    // Attenuation (distance to 0% light)
    float m_attenuation;
    
    // Number of passes (textures, matrices, etc.)
    unsigned int m_passes;
    
    // Completed depth texture(s)
    Texture** m_depthTextures;
    
    // Depth pass render
    DepthPass* m_depthPass;
    
    // Our cached camera component(s)
    CameraComponent** m_cameras;
};

#endif
