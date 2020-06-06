
#ifndef terrainquad_h
#define terrainquad_h

#include <Core/GigaObject.h>
#include <Render/RenderComponent.h>

class GIGA_API TerrainQuad : public RenderComponent {
public:
    TerrainQuad();
    ~TerrainQuad();
    
    GIGA_CLASS_NAME("TerrainQuad");
    
    /**
     * Load / unload the data for this quad section
     */
    void Load(Texture2D* heightMap, Texture2D* normalMap, vector2 fileOffset, float width);
    void Unload();
    
    /**
     * Get / set LOD
     */
    int32_t GetLOD() { return m_currentLOD; }
    void SetLOD(vector3 cameraPosition);
    
protected:
    // Width
    uint32_t m_width;
    
    // Total width (of component)
    uint32_t m_totalWidth;
    
    // Current LOD
    int32_t m_currentLOD;
    
    // Offset into file (for texturing)
    vector2 m_fileOffset;
    
    // Our copy of the data for this quad
    unsigned char* m_data;
    float* m_normals;
};

#endif
