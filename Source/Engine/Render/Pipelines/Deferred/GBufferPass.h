
#ifndef gbufferpass_h
#define gbufferpass_h

#include <Render/RenderPass.h>
#include <Render/MeshComponent.h>

class GIGA_API GBufferPass : public RenderPass {
public:
    GBufferPass() = default;
    ~GBufferPass() = default;
    
    GIGA_CLASS_NAME("GBufferPass");
    
    /**
     * Initialize render pass (called on window size change, should re-initialize)
     */
    virtual void Initialize(int width, int height);
    
    /**
     * Render
     */
    virtual void Render(Scene* scene);
    
protected:
    void RecursiveRender(MeshComponent* rc, matrix4 view, matrix4 parent, Scene* scene);
};

#endif 
