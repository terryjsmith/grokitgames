
#include <Render/Renderable.h>

Renderable::Renderable() {
    vertexBuffer = 0;
    indexBuffer = 0;
    diffuseTexture = 0;
    normalTexture = 0;
    emissiveTexture = 0;
    ambientTexture = 0;
    specularTexture = 0;
    material = 0;
}

Renderable::~Renderable() {
    if(children.size()) {
        auto it = children.begin();
        for(; it != children.end(); it++) {
            delete(*it);
        }
        
        children.clear();
    }
}
