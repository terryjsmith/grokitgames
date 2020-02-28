
#ifndef openglrendersystem_h
#define openglrendersystem_h

#include <Render/RenderSystem.h>
#include <Render/OpenGL/OpenGLVertexFormat.h>
#include <Render/OpenGL/OpenGLVertexBuffer.h>
#include <Render/OpenGL/OpenGLShaderProgram.h>
#include <Render/OpenGL/OpenGLTexture2D.h>
#include <Render/OpenGL/OpenGLTexture3D.h>
#include <Render/OpenGL/OpenGLFramebuffer.h>
#include <Render/OpenGL/OpenGLIndexBuffer.h>

class GIGA_API OpenGLRenderSystem : public RenderSystem {
public:
    OpenGLRenderSystem() = default;
    ~OpenGLRenderSystem() = default;
    
    /**
     * Initialize system
     */
    virtual void Initialize();
    virtual void Initialize(int width, int height, bool fullscreen);
    
    /**
     * Create all of the things
     */
    VertexBuffer* CreateVertexBuffer() { return(new OpenGLVertexBuffer()); }
    IndexBuffer* CreateIndexBuffer() { return(new OpenGLIndexBuffer()); }
    VertexFormat* CreateVertexFormat() { return(new OpenGLVertexFormat()); }
    ShaderProgram* CreateShaderProgram() { return(new OpenGLShaderProgram()); }
    Framebuffer* CreateFramebuffer() { return(new OpenGLFramebuffer()); }
    Texture2D* CreateTexture2D() { return(new OpenGLTexture2D()); }
    Texture3D* CreateTexture3D() { return(new OpenGLTexture3D()); }
    
    /**
     * Clear buffers
     */
    void SetClearColor(vector4 color);
    void Clear(int bitmask);
    
    /**
     * Draw + indexed draw
     */
    void Draw(int type, int elements);
    void DrawIndexed(int type, int elements);
    
    /**
     * Depth testing
     */
    void EnableDepthTest(int type);
    void DisableDepthTest();
    
    /**
     * Blending
     */
    void EnableBlending();
    void DisableBlending();
    void SetBlendFunc(int sourceFactor, int destFactor);
    
    /**
     * Set viewport (normally done by framebuffer)
     */
    void SetViewport(int width, int height);
    
    /**
     * Set render type (front/back/none)
     */
    void SetDrawBuffer(int buffer);
    
    /**
     * Use default framebuffer
     */
    void UseDefaultFramebuffer();
};

#endif
