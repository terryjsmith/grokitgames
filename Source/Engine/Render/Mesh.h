
#ifndef mesh_h
#define mesh_h

#include <Core/GigaObject.h>
#include <IO/ResourceObject.h>
#include <Render/Renderable.h>
#include <Render/Animation.h>
#include <Render/Bone.h>

GIGA_CLASS()
class GIGA_API Mesh : public ResourceObject {
public:
    Mesh();
    ~Mesh() = default;
    
    GIGA_CLASS_NAME("Mesh");
    
    void ProcessData();
    
public:
    // Mesh name (loaded from file)
    std::string name;
    
    // Renderable
    Renderable* renderable;
    
    // Animations
    std::map<std::string, Animation*> animations;
    
    // Bones
    std::map<std::string, Bone*> bones;
};

#endif
