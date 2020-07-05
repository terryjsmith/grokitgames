
#include <Render/RenderPipeline.h>

RenderPipeline::RenderPipeline() {
    
}

RenderPipeline::~RenderPipeline() {
    auto pi = m_renderPasses.begin();
    for(; pi != m_renderPasses.end(); pi++) {
        delete(*pi);
    }
    m_renderPasses.clear();
}

void RenderPipeline::AddPass(RenderPass* pass) {
    m_renderPasses.push_back(pass);
}
