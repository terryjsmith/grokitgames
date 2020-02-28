
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
    
    GL_CHECK(glEnableVertexAttribArray(index));
    GL_CHECK(glVertexAttribPointer(index, attr->size, GL_FLOAT, GL_FALSE, sizeof(float) *  m_vertexSize, (void*)(sizeof(float) * attr->offset)));
    
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
