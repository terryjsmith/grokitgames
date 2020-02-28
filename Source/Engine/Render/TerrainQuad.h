
#ifndef terrainquad_h
#define terrainquad_h

#include <Core/GigaObject.h>
#include <Render/Renderable.h>
#include <Render/BoundingBox.h>

class GIGA_API TerrainQuad : public Renderable {
public:
    TerrainQuad();
    ~TerrainQuad();
    
    /**
     * Load / unload the data for this quad section
     */
    void Load(Texture2D* hmap, vector2 offset, vector2 quadDimensions);
    void Unload();
    
    /**
     * Get / set LOD
     */
    int32_t GetLOD() { return m_currentLOD; }
    void SetLOD(vector3 cameraPosition);
    
    /**
     * Get bounding box
     */
    BoundingBox* GetBoundingBox() { return &m_aabb; }
    
    /**
     * Get offset
     */
    vector3 GetOffset() { return vector3(m_offset.x, 0, m_offset.y); }
    
protected:
    // Bounding box
    BoundingBox m_aabb;
    
    // Width
    uint32_t m_width;
    
    // Total width (of component)
    uint32_t m_totalWidth;
    
    // Offset
    vector2 m_offset;
    
    // Current LOD
    int32_t m_currentLOD;
    
    // Our copy of the data for this quad
    unsigned char* m_data;
};

#endif
