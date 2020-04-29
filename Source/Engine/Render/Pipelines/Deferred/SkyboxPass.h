
#ifndef skyboxpass_h
#define skyboxpass_h

#include <Render/RenderPass.h>
#include <Render/Skybox.h>

class GIGA_API SkyboxPass : public RenderPass {
public:
    SkyboxPass() = default;
    ~SkyboxPass() = default;
    
    GIGA_CLASS_NAME("SkyboxPass");
    
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
    void SetDepthTexture(Texture2D* texture);
    
protected:
    Texture2D* m_texture;
};


#endif
