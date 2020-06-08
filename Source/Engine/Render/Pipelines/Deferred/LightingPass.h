
#ifndef lightingpass_h
#define lightingpass_h

#include <Render/RenderPass.h>
#include <Render/GaussianBlur.h>

class GIGA_API LightingPass : public RenderPass {
public:
    LightingPass();
    ~LightingPass() = default;
    
    GIGA_CLASS_NAME("LightingPass");
    
    /**
     * Initialize render pass (called on window size change, should re-initialize)
     */
    virtual void Initialize(int width, int height);
    
    /**
     * Render
     */
    virtual void Render(Scene* scene);
    
    /**
     * Set normal texture
     */
    void SetNormalTexture(Texture2D* texture) { m_normalTexture = texture; }
    void SetPositionTexture(Texture2D* texture) { m_positionTexture = texture; }
    void SetMaterialTexture(Texture2D* texture) { m_materialTexture = texture; }
    
protected:
    Texture2D* m_normalTexture;
    Texture2D* m_positionTexture;
    Texture2D* m_materialTexture;
    
    Texture3D* m_null;
    
    // Window width and height
    int m_windowWidth, m_windowHeight;
    
    // Internal vertex buffer and format for screen pass
    VertexBuffer* m_vertexBuffer;
    VertexFormat* m_vertexFormat;
};

#endif 
