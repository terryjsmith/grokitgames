
#ifndef terrainsystem_h
#define terrainsystem_h

#include <Core/System.h>
#include <Render/TerrainComponent.h>

class GIGA_API TerrainSystem : public System {
public:
    TerrainSystem();
    ~TerrainSystem() = default;
    
    GIGA_CLASS_NAME("TerrainSystem");
    
    /**
     * Iterate over the terrain components in the world and load / unload / set LOD as appropriate
     */
    void Update(float delta);
    
    /**
     * Get / set max quad size
     */
    void SetMaxQuadSize(uint32_t size) { m_maxQuadSize = size; }
    uint32_t GetMaxQuadSize() { return m_maxQuadSize; }
    
    /**
     * Get / set the distance from the camera to be loaded / unloaded
     */
    void SetLoadingDistance(uint32_t distance) { m_loadDistance = distance; }
    uint32_t GetLoadingDistance() { return m_loadDistance; }
    
    /**
     * Get / set the maximum LOD (max number of vertices skipped)
     */
    void SetMaxLOD(uint32_t lod) { m_maxLOD = lod; }
    uint32_t GetMaxLOD() { return m_maxLOD; }
    
    /**
     * Get terrain scaling factor
     */
    float GetTerrainScale() { return m_terrainScale; }
    void SetTerrainScale(float terrainScale) { m_terrainScale = terrainScale; }
    
protected:
    // Max size of quad
    uint32_t m_maxQuadSize;
    
    // Distance from camera for load / unload
    uint32_t m_loadDistance;
    
    // Max LOD (max vertices skipped)
    uint32_t m_maxLOD;
    
    // Terrain scaling factor
    float m_terrainScale;
};

#endif
