
#ifndef combinepass_h
#define combinepass_h

#include <Render/RenderPass.h>

class GIGA_API CombinePass : public RenderPass {
public:
    CombinePass();
    ~CombinePass() = default;
    
    GIGA_CLASS_NAME("CombinePass");
    
    /**
     * Initialize render pass (called on window size change, should re-initialize)
     */
    virtual void Initialize(int width, int height);
    
    /**
     * Render
     */
    virtual void Render(Scene* scene);
    
    /**
     * Set up textures
     */
    void SetDiffuseTexture(Texture2D* diffuse) { m_diffuseTexture = diffuse; }
    void SetLightingTexture(Texture2D* lighting) { m_lightingTexture = lighting; }
    
protected:
    Texture2D* m_diffuseTexture;
    Texture2D* m_lightingTexture;
    
    // Window width and height
    int m_windowWidth, m_windowHeight;
    
    // Internal vertex buffer and format for screen pass
    VertexBuffer* m_vertexBuffer;
    VertexFormat* m_vertexFormat;
};

#endif 
