#ifndef texture2d_h
#define texture2d_h

#include <Render/Texture.h>

/**
 * Our class for storing textures loaded from disk
 */
GIGA_CLASS()
class GIGA_API Texture2D : public Texture {
public:
    Texture2D() = default;
    ~Texture2D() = default;
    
    GIGA_CLASS_NAME("Texture2D");
    
    /**
     * Generate mip maps
     */
    virtual void GenerateMipMaps() { }
};

#endif
