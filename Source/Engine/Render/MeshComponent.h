
#ifndef meshcomponent_h
#define meshcomponent_h

#include <Render/RenderComponent.h>
#include <Render/Mesh.h>
#include <Render/AnimationInstance.h>

GIGA_CLASS()
class GIGA_API MeshComponent : public RenderComponent {
public:
    MeshComponent();
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
    // Loaded mesh
    Mesh* mesh;
    
    // Currently playing animation
    AnimationInstance* animation;
    
    // Instanced?
    bool instanced;
    int instances;
};

#endif
