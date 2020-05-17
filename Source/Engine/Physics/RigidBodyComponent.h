
#ifndef rigidbodycomponent_h
#define rigidbodycomponent_h

#include <Core/Component.h>
#include <Core/Transform.h>
#include <Physics/CollisionShape.h>

GIGA_CLASS()
class GIGA_API RigidBodyComponent : public Component {
public:
    RigidBodyComponent();
    ~RigidBodyComponent();
    
    GIGA_CLASS_NAME("RigidBodyComponent");
    
    /**
     * Get transform
     */
    GIGA_FUNCTION() Transform* GetTransform() { return m_transform; }
    
protected:
    Transform* m_transform;
    
    // Collision shape
    CollisionShape* m_shape;
};

#endif
