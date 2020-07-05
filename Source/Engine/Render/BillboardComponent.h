
#ifndef billboard_h
#define billboard_h

#include <Render/Texture2D.h>
#include <Render/MeshComponent.h>
#include <Core/DataRecord.h>

class GIGA_API BillboardComponent : public MeshComponent {
public:
    BillboardComponent();
    ~BillboardComponent() = default;
    
    /**
     * Create
     */
    void Create(Texture2D* tex, float size);
    
    /**
     * Any pre-render work that needs to be done can be done here
     */
    virtual void PreRender(Scene* scene);
    
    /**
     * Initialize
     */
    void Initialize();
    
    /**
     * Serialize / deserialize
     */
    virtual void Serialize(DataRecord* record);
    virtual void Deserialize(DataRecord* record);
    
protected:
    // Billboarded texture
    Texture2D* m_texture;
    
    // Size
    float m_size;
};

#endif
