
#ifndef openglvertexbuffer_h
#define openglvertexbuffer_h

#include <Render/VertexBuffer.h>

class GIGA_API OpenGLVertexBuffer : public VertexBuffer {
public:
    OpenGLVertexBuffer();
    ~OpenGLVertexBuffer() = default;
    
    void Create(VertexFormat* type, int count, float* data, bool dynamic);
    void SetData(int count, float* data);
    
    void Bind();
    void Unbind();
    
protected:
    // internal buffer
    unsigned int m_buffer;
};

#endif
