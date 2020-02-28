
#ifndef shader_h
#define shader_h

#include <IO/ResourceObject.h>

/**
 * A vertex or pixel/fragment shader
 */
GIGA_CLASS()
class GIGA_API Shader : public ResourceObject {
public:
    Shader() = default;
    ~Shader() = default;
    
    GIGA_CLASS_NAME("Shader");
};

#endif
