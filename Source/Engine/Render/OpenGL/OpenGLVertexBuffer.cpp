
#include <Render/OpenGL/OpenGLVertexBuffer.h>
#include <Render/OpenGL/OpenGL.h>

OpenGLVertexBuffer::OpenGLVertexBuffer() {
    m_buffer = 0;
}

void OpenGLVertexBuffer::Create(VertexFormat* type, int count, float* data, bool dynamic) {
    m_type = type;
    m_dynamic = dynamic;
    
    m_vertexData = (float*)malloc(sizeof(float) * type->GetVertexSize() * count);
    memcpy(m_vertexData, data, sizeof(float) * type->GetVertexSize() * count);
    m_vertexCount = count;
    
    GL_CHECK(glGenBuffers(1, &m_buffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * type->GetVertexSize() * m_vertexCount, data, m_dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW));
}

void OpenGLVertexBuffer::SetData(int count, float* data) {
    if(m_buffer == 0) {
        GL_CHECK(glGenBuffers(1, &m_buffer));
    }
    
    m_dynamic = true;
    
    m_vertexData = (float*)malloc(sizeof(float) * m_type->GetVertexSize() * count);
    memcpy(m_vertexData, data, sizeof(float) * m_type->GetVertexSize() * count);
    m_vertexCount = count;
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_type->GetVertexSize() * m_vertexCount, data, GL_STREAM_DRAW));
}

void OpenGLVertexBuffer::Bind() {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
    
    int size = 0;
    GL_CHECK(glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size));
}

void OpenGLVertexBuffer::Unbind() {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
