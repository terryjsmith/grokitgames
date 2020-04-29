#ifndef texture3d_h
#define texture3d_h

#include <Render/Texture2D.h>

/**
 * Our class for storing 3D (cubemap) textures
 */
class GIGA_API Texture3D : public Texture {
public:
    Texture3D() = default;
    virtual ~Texture3D() = default;
    
    GIGA_CLASS_NAME("Texture3D");
    
    virtual void Load(int slot, std::string filename) { }
};

#endif
