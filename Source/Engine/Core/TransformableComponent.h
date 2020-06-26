
#ifndef transformablecomponent_h
#define transformablecomponent_h

#include <Core/Transform.h>
#include <Core/Component.h>

class GIGA_API TransformableComponent : public Component {
public:
    TransformableComponent();
    ~TransformableComponent();
    
    GIGA_FUNCTION() Transform* GetTransform() { return m_transform; }
    
    /**
     * Callback functions for add/remove from Entity
     */
    virtual void onComponentAdded(Component* c);
    virtual void onEntitySet();
    
    /**
     * Serialization
     */
    virtual void Serialize(DataRecord* record);
    virtual void Deserialize(DataRecord* record);
    
protected:
    /**
     * Set transform component hierarchy from parent
     */
    void SetParent();
    
protected:
    Transform* m_transform;
};

#endif
