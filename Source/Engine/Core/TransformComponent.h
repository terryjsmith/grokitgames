
#ifndef transformcomponent_h
#define transformcomponent_h

#include <Core/TransformableComponent.h>

GIGA_CLASS()
class GIGA_API TransformComponent : public TransformableComponent {
public:
    TransformComponent() = default;
    ~TransformComponent() = default;
    
    GIGA_CLASS_NAME("TransformComponent");
};

#endif
