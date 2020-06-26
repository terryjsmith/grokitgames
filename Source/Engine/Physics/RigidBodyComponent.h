
#ifndef rigidbodycomponent_h
#define rigidbodycomponent_h

#include <Core/TransformableComponent.h>
#include <Physics/CollisionShape.h>

GIGA_CLASS()
class GIGA_API RigidBodyComponent : public TransformableComponent {
public:
    RigidBodyComponent();
    ~RigidBodyComponent();
    
    GIGA_CLASS_NAME("RigidBodyComponent");

protected:
    // Collision shape
    CollisionShape* m_shape;
};

#endif
