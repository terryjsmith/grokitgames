
#include <Render/Pipeline.h>

void Pipeline::AddPass(RenderPass* pass) {
    m_renderPasses.push_back(pass);
}
