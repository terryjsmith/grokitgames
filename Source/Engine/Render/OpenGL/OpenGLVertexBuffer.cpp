
#include <Render/OpenGL/OpenGLVertexBuffer.h>
#include <Render/OpenGL/OpenGL.h>

OpenGLVertexBuffer::OpenGLVertexBuffer() {
    m_buffer = 0;
    m_vertexData = 0;
}

void OpenGLVertexBuffer::Create(VertexFormat* type, int count, float* data, bool dynamic, int size) {
    m_type = type;
    m_dynamic = dynamic;
    
    int bufferSize = size == 0 ? m_type->GetVertexSize() * count : size;
    m_vertexCount = count;
    m_bufferSize = bufferSize;
    
    GL_CHECK(glGenBuffers(1, &m_buffer));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferSize, data, m_dynamic ? GL_STREAM_DRAW : GL_STATIC_DRAW));
}

void OpenGLVertexBuffer::SetData(int size, int offset, float* data) {
    if(m_buffer == 0) {
        GL_CHECK(glGenBuffers(1, &m_buffer));
    }
    
    m_dynamic = true;
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * offset, sizeof(float) * size, data));
}

void OpenGLVertexBuffer::Bind() {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_buffer));
    
    int size = 0;
    GL_CHECK(glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size));
}

void OpenGLVertexBuffer::Unbind() {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
