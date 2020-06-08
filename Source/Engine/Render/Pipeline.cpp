
#include <Render/Pipeline.h>

Pipeline::Pipeline() {
    
}

Pipeline::~Pipeline() {
    auto pi = m_renderPasses.begin();
    for(; pi != m_renderPasses.end(); pi++) {
        delete(*pi);
    }
    m_renderPasses.clear();
}

void Pipeline::AddPass(RenderPass* pass) {
    m_renderPasses.push_back(pass);
}
