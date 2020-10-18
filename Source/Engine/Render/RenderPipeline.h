
#ifndef renderpipeline_h
#define renderpipeline_h

#include <Core/GigaObject.h>
#include <Render/RenderPass.h>

class GIGA_API RenderPipeline : public GigaObject {
public:
    RenderPipeline();
    ~RenderPipeline();

    GIGA_CLASS_NAME("RenderPipeline");
    
    /**
     * Initialize render pipeline
     */
    virtual void Initialize(int width, int height) { }
    
    /**
     * Render scene
     */
    virtual void Render(Scene* scene) { } 
    
    /**
     * Add render pass
     */
    void AddPass(RenderPass* pass);
    
    /**
     * Get textures
     */
    virtual Texture2D* GetDepthTexture() { return(0); }
    virtual Texture2D* GetIndexTexture() { return(0); }
    
protected:
    // Render passes
    Array<RenderPass*> m_renderPasses;
};

#endif
