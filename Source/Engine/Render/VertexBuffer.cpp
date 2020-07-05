
#include <Render/VertexBuffer.h>

VertexBuffer::VertexBuffer() {
    m_bufferSize = 0;
    m_type = 0;
    m_dynamic = false;
    m_vertexData = 0;
    m_vertexCount = 0;
}

VertexBuffer::~VertexBuffer() {
    if (m_type) {
        delete m_type;
        m_type = 0;
    }
    
    if(m_vertexData) {
        free(m_vertexData);
        m_vertexData = 0;
        
        m_vertexCount = 0;
    }
}

void VertexBuffer::Create(VertexFormat* type, int count, float* data, bool dynamic, int size) {
    int bufferSize = size == 0 ? type->GetVertexSize() * count : size;
    m_vertexData = (float*)malloc(sizeof(float) * bufferSize);
    memcpy(m_vertexData, data, sizeof(float) * bufferSize);
    m_vertexCount = count;
    m_bufferSize = bufferSize;
}

void VertexBuffer::SetData(int size, int offset, float* data) {
    assert(offset + size < m_bufferSize);
    
    if(m_vertexData) {
        delete m_vertexData;
    }

    memcpy(m_vertexData + offset, data, sizeof(float) * size);
}
