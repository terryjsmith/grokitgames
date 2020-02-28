
#ifndef scene_h
#define scene_h

#include <Core/GigaObject.h>
#include <Render/RenderComponent.h>
#include <Render/CameraComponent.h>

class Scene : public GigaObject {
public:
    Scene() = default;
    ~Scene() = default;
    
    GIGA_CLASS_NAME("Scene");
    
public:
    std::vector<RenderComponent*> renderables;
    CameraComponent* camera;
};

#endif
