
#ifndef renderpass_h
#define renderpass_h

#include <Core/GigaObject.h>
#include <Render/Scene.h>
#include <Render/Framebuffer.h>
#include <Render/ShaderProgram.h>

class GIGA_API RenderPass : public GigaObject {
public:
    RenderPass();
    virtual ~RenderPass() = default;
    
    /**
     * Initialize render pass (called on window size change, should re-initialize)
     */
    virtual void Initialize(int width, int height) { }
    
    /**
     * Render
     */
    virtual void Render(Scene* scene) { }
    
    /**
     * Get framebuffer
     */
    virtual Framebuffer* GetFramebuffer(int index);
    
protected:
    // Framebuffers
    std::vector<Framebuffer*> m_framebuffers;
    
    // Shader program
    ShaderProgram* m_program;
};

#endif
