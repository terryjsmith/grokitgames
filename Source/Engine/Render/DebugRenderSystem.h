
#ifndef debugrendersystem_h
#define debugrendersystem_h

#include <Render/RenderSystem.h>
#include <Core/MathTypes.h>

class GIGA_API DebugRenderSystem : public RenderSystem {
public:
    DebugRenderSystem() = default;
    ~DebugRenderSystem() = default;
    
    /**
     * Clear lists
     */
    void Clear();
    
    /**
     * Add line
     */
    void AddLine(vector3 p1, vector3 p2);

protected:
    Array<vector3> m_linePoints;
};

#endif
