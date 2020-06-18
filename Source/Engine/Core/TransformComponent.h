
#ifndef transformcomponent_h
#define transformcomponent_h

#include <Core/Component.h>
#include <Core/Transform.h>

GIGA_CLASS()
class GIGA_API TransformComponent : public Component {
public:
    TransformComponent();
    ~TransformComponent();
    
    GIGA_CLASS_NAME("TransformComponent");
    
    Transform* GetTransform() { return m_transform; }
    
    /**
     * Serialize / deserialize
     */
    void Serialize(DataRecord* record);
    void Deserialize(DataRecord* record);
    
protected:
    Transform* m_transform;
};

#endif
