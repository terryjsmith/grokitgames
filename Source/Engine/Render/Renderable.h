
#ifndef renderable_h
#define renderable_h

#include <Render/VertexBuffer.h>
#include <Render/IndexBuffer.h>
#include <Render/Texture2D.h>
#include <Render/Material.h>
#include <Render/AABB.h>

class GIGA_API Renderable {
public:
    Renderable();
    virtual ~Renderable();
    
public:
    std::vector<Renderable*> children;
    
    VertexBuffer* vertexBuffer;
    IndexBuffer* indexBuffer;
    
    Texture2D* diffuseTexture;
    Texture2D* normalTexture;
    Texture2D* specularTexture;
    Texture2D* ambientTexture;
    Texture2D* emissiveTexture;
    
    Material* material;
    
    AABB* aabb;
};

#endif
