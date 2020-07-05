
#ifndef openglvertexformat_h
#define openglvertexformat_h

#include <Render/VertexFormat.h>

class GIGA_API OpenGLVertexFormat : public VertexFormat {
public:
    OpenGLVertexFormat() = default;
    ~OpenGLVertexFormat() = default;
    
    void Initialize();
    void Destroy();
    
    /**
     * Enable a particular vertex attribute (if this vertex type has it)
     */
    bool EnableAttribute(int index, int attrib);
    bool DisableAttribute(int index);
    
    /**
     * Bind/unbind this vertex type
     */
    void Bind();
    void Unbind();
    
    /**
     * Set divisor/stride per index (instancing)
     */
    void SetDivisor(int index, int divisor);
    
protected:
    // Internal handle
    unsigned int m_vertexArray;
};

#endif
