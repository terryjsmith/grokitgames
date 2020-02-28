
#ifndef meshcomponent_h
#define meshcomponent_h

#include <Render/RenderComponent.h>
#include <Render/Mesh.h>
#include <Core/Serializable.h>

GIGA_CLASS()
class GIGA_API MeshComponent : public RenderComponent, public Serializable {
public:
    MeshComponent() = default;
    ~MeshComponent() = default;
    
    GIGA_CLASS_NAME("MeshComponent");
    
    /**
     * Create from mesh
     */
    void Initialize(Mesh* mesh);
    
    /**
     * Serialize / deserialize
     */
    virtual void Serialize(DataRecord* record);
    virtual void Deserialize(DataRecord* record);
    
public:
    // Children
    std::vector<MeshComponent*> children;
    
    Mesh* m_mesh;
};

#endif
