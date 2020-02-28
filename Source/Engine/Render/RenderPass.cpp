
#include <Render/RenderPass.h>

RenderPass::RenderPass() {
    m_program = 0;
}

Framebuffer* RenderPass::GetFramebuffer(int index) {
    return(m_framebuffers[index]);
}
