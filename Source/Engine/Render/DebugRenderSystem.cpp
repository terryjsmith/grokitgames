
#include <Render/DebugRenderSystem.h>

void DebugRenderSystem::AddLine(vector3 p1, vector3 p2) {
    m_linePoints.push_back(p1);
    m_linePoints.push_back(p2);
}

void DebugRenderSystem::Clear() {
    m_linePoints.clear();
}
