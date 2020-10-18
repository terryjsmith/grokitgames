
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
    
    /**
     * Unproject 2D screen coordinates to a 3D world position
     */
    vector3 Unproject(vector2 screenCoords);
    
    /**
     * Project a 3D world position to 2D screen coordinates
     */
    vector2 Project(vector3 worldCoords);
    
public:
    Array<RenderComponent*> renderables;
    CameraComponent* camera;
};

#endif
