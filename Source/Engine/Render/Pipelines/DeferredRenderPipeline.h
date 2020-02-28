
#ifndef deferredrenderpipeline_h
#define deferredrenderpipeline_h

#include <Render/Pipeline.h>

class GIGA_API DeferredRenderPipeline : public Pipeline {
public:
    DeferredRenderPipeline() = default;
    ~DeferredRenderPipeline() = default;
    
    GIGA_CLASS_NAME("DeferredRenderPipeline");
    
    /**
     * Initialize render pipeline
     */
    void Initialize(int width, int height);
    
    /**
     * Render scene
     */
    void Render(Scene* scene);
};

#endif 
