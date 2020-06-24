
#ifndef terraincomponent_h
#define terraincomponent_h

#include <Render/TerrainQuad.h>
#include <Render/RenderComponent.h>
#include <Render/Scene.h>

class GIGA_API TerrainComponent : public RenderComponent {
public:
    TerrainComponent();
    ~TerrainComponent() = default;
    
    GIGA_CLASS_NAME("TerrainComponent");
    
    /**
     * Load a terrain component from a heightmap
     */
    void Load(std::string hmapFile, std::string nmapFile, std::string splatFile, Array<std::string> textureFiles, vector3 offset);
    void Unload();
    
    /**
     * Update (LOD)
     */
    void Update(Scene* scene);
    
    /**
     * Get textures
     */
    Texture2D* GetSplatMap() { return m_splat; }
    Texture2D* GetTexture(int index) { return m_textures[index]; }
    int GetNumTextures() { return m_numTextures; }
    
protected:
    Texture2D* m_textures[4];
    int m_numTextures;
    
    Texture2D* m_splat;
};

#endif
