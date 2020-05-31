
#ifndef mesh_h
#define mesh_h

#include <Core/GigaObject.h>
#include <IO/ResourceObject.h>
#include <Render/Renderable.h>
#include <Render/Animation.h>
#include <Render/Bone.h>
#include <Render/Node.h>

GIGA_CLASS()
class GIGA_API Mesh : public ResourceObject {
public:
    Mesh();
    ~Mesh() = default;
    
    GIGA_CLASS_NAME("Mesh");
    
    /**
     * Processed data loaded from file
     */
    void ProcessData();
    
public:
    // Mesh name (loaded from file)
    std::string name;
    
    // Renderable
    Renderable* renderable;
    
    // Animations
    std::map<std::string, Animation*> animations;
    
    // Global inverse matrix
    matrix4 globalInverseMatrix;
    
    // Bones
    std::map<std::string, Bone*> bones;
    
    // Node hierarchy
    Node* nodes;
};

#endif
