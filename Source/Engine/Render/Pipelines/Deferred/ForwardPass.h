
#ifndef forwardpass_h
#define forwardpass_h

#include <Render/RenderPass.h>
#include <Render/MeshComponent.h>

class GIGA_API ForwardPass : public RenderPass {
public:
    ForwardPass();
    ~ForwardPass();
    
    GIGA_CLASS_NAME("ForwardPass");
    
    /**
     * Initialize render pass (called on window size change, should re-initialize)
     */
    virtual void Initialize(int width, int height);
    
    /**
     * Render
     */
    virtual void Render(Scene* scene);
    
    /**
     * Set texture from previous passes
     */
    void SetInputTexture(Texture2D* texture);
    void SetAuxTexture(Texture2D* texture);
    void SetDepthTexture(Texture2D* texture);
    
protected:
    void RecursiveRender(MeshComponent* rc, matrix4 view, matrix4 parent, ShaderProgram* program);
    
protected:
    Texture2D* m_texture;
    Texture2D* m_texture2;
};

#endif
