
#ifndef rendertoscreenpass_h
#define rendertoscreenpass_h

#include <Render/RenderPass.h>

class GIGA_API RenderToScreenPass : public RenderPass {
public:
    RenderToScreenPass();
    ~RenderToScreenPass() = default;
    
    GIGA_CLASS_NAME("RenderToScreenPass");
    
    /**
     * Initialize render pass (called on window size change, should re-initialize)
     */
    virtual void Initialize(int width, int height, int x, int y);
    
    /**
     * Render
     */
    virtual void Render(Scene* scene);
    
    /**
     * Set input texture (to render to screen)
     */
    void SetInputTexture(Texture2D* tex) { m_inputTexture = tex; }
    
protected:
    void RecursiveRender(RenderComponent* rc, matrix4 view, matrix4 parent);
    
protected:
    // Window width and height
    int m_windowWidth, m_windowHeight;
    int m_windowX, m_windowY;
    
    // Internal vertex buffer and format for screen pass
    VertexBuffer* m_vertexBuffer;
    VertexFormat* m_vertexFormat;
    
    // Input texture to render to screen
    Texture2D* m_inputTexture;
};

#endif
