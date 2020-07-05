
#include <Render/OpenGL/OpenGLVertexFormat.h>
#include <Render/OpenGL/OpenGL.h>

void OpenGLVertexFormat::Initialize() {
    GL_CHECK(glGenVertexArrays(1, &m_vertexArray));
}

void OpenGLVertexFormat::Destroy() {
    if (m_vertexArray) {
        GL_CHECK(glDeleteVertexArrays(1, &m_vertexArray));
    }
}

bool OpenGLVertexFormat::EnableAttribute(int index, int attrib) {
    if ((m_attribBitmask & attrib) == 0) {
        return(false);
    }
    
    std::map<int, VertexAttrib*>::iterator ai = m_attribs.find(attrib);
    VertexAttrib* attr = ai->second;
    
    int strideLength = ai->second->stride == 0 ? m_vertexSize : ai->second->stride;
    GL_CHECK(glEnableVertexAttribArray(index));
    GL_CHECK(glVertexAttribPointer(index, attr->size, GL_FLOAT, GL_FALSE, sizeof(float) *  strideLength, (void*)(sizeof(float) * attr->offset)));
    
    return(true);
}

bool OpenGLVertexFormat::DisableAttribute(int index) {
    GL_CHECK(glDisableVertexAttribArray(index));
    return(true);
}

void OpenGLVertexFormat::Bind() {
    GL_CHECK(glBindVertexArray(m_vertexArray));
}

void OpenGLVertexFormat::Unbind() {
    GL_CHECK(glBindVertexArray(0));
}

void OpenGLVertexFormat::SetDivisor(int index, int divisor) {
    GL_CHECK(glVertexAttribDivisor(index, divisor));
}
