
#include <Render/RenderPass.h>

RenderPass::RenderPass() {
    m_program = 0;
}

RenderPass::~RenderPass() {
    auto fi = m_framebuffers.begin();
    for(; fi != m_framebuffers.end(); fi++) {
        delete (*fi);
    }
    m_framebuffers.clear();
}

Framebuffer* RenderPass::GetFramebuffer(int index) {
    return(m_framebuffers[index]);
}
