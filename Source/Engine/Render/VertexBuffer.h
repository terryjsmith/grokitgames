
#ifndef vertexbuffer_h
#define vertexbuffer_h

#include <Render/VertexFormat.h>

class GIGA_API VertexBuffer {
public:
    VertexBuffer();
    virtual ~VertexBuffer();
    
    virtual void Create(VertexFormat* type, int count, float* data, bool dynamic, int size = 0);
    virtual void SetData(int size, int offset, float* data);
    
    virtual void Bind() { }
    virtual void Unbind() { }
    
    void SetFormat(VertexFormat* type) { m_type = type; }
    VertexFormat* GetFormat() { return m_type; }
    
    int GetCount() { return m_vertexCount; }
    float* GetData() { return m_vertexData; }
    
protected:
    VertexFormat* m_type;
    bool m_dynamic;
    
    float* m_vertexData;
    int m_vertexCount;
    int m_bufferSize;
};

#endif

