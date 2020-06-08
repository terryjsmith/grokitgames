
#ifndef pipeline_h
#define pipeline_h

#include <Core/GigaObject.h>
#include <Render/RenderPass.h>

class GIGA_API Pipeline : public GigaObject {
public:
    Pipeline();
    ~Pipeline();
    
    /**
     * Initialize render pipeline
     */
    virtual void Initialize(int width, int height) { }
    
    /**
     * Render scene
     */
    virtual void Render(Scene* scene) = 0;
    
    /**
     * Add render pass
     */
    void AddPass(RenderPass* pass);
    
protected:
    // Render passes
    std::vector<RenderPass*> m_renderPasses;
};

#endif
