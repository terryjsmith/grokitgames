
#ifndef terrainpass_h
#define terrainpass_h

#include <Render/RenderPass.h>
#include <Render/MeshComponent.h>
#include <Render/TerrainComponent.h>

class GIGA_API TerrainPass : public RenderPass {
public:
    TerrainPass() = default;
    ~TerrainPass();
    
    GIGA_CLASS_NAME("TerrainPass");
    
    /**
     * Initialize render pass (called on window size change, should re-initialize)
     */
    virtual void Initialize(int width, int height);
    
    /**
     * Render
     */
    virtual void Render(Scene* scene);
    
    /**
     * Set textures
     */
    void SetNormalTexture(Texture2D* texture);
    void SetPositionTexture(Texture2D* texture);
    void SetDiffuseTexture(Texture2D* texture);
    void SetDepthTexture(Texture2D* texture);
    void SetAuxTexture(Texture2D* texture);
    
protected:
    void RecursiveRender(TerrainQuad* rc, matrix4 view, matrix4 parent, Scene* scene);
};

#endif 
