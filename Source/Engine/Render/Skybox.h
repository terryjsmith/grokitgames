
#ifndef skybox_h
#define skybox_h

#include <Render/RenderComponent.h>
#include <Render/Texture3D.h>

class GIGA_API Skybox : public RenderComponent {
public:
    Skybox() = default;
    ~Skybox() = default;
    
    GIGA_CLASS_NAME("Skybox");
    
    void Initialize(std::string prefix);
    
public:
    Texture3D* cubeMap;
};

#endif
