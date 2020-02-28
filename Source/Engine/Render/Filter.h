
#ifndef filter_h
#define filter_h

#include <Core/GigaObject.h>
#include <Render/Texture2D.h>
#include <Render/Framebuffer.h>
#include <Render/ShaderProgram.h>
#include <Render/VertexBuffer.h>

/**
 * Filter takes a texture and applied a "filter" to it
 */
class GIGA_API Filter : public GigaObject {
public:
    Filter();
    ~Filter();
    
    GIGA_CLASS_NAME("Filter");
    
    // Initialize
    void Initialize(std::string shader, int width, int height);
    
    // Set input texture
    void SetInputTexture(Texture2D* texture) { m_inputTexture = texture; }
    
    // "Apply"/render
    virtual void Apply() { }
    
    // Get output texture
    Texture2D* GetOutputTexture() { return m_outputTexture; }
    
protected:
    // Pointer to input texture
    Texture2D* m_inputTexture;
    
    // Output texture
    Texture2D* m_outputTexture;
    
    // Framebuffer
    Framebuffer* m_framebuffer;
    
    // Shader
    ShaderProgram* m_program;
    
    // Width and height
    int m_width, m_height;
    
    // Internal vertex buffer and format for "screen" pass
    VertexBuffer* m_vertexBuffer;
    VertexFormat* m_vertexFormat;
};

#endif
