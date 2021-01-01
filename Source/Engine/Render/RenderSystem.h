
#ifndef rendersystem_h
#define rendersystem_h

#include <Core/System.h>
#include <Render/RenderPipeline.h>
#include <Render/VertexFormat.h>
#include <Render/VertexBuffer.h>
#include <Render/Scene.h>
#include <Render/ShaderProgram.h>
#include <Render/Texture2D.h>
#include <Render/Texture3D.h>
#include <Render/Framebuffer.h>
#include <Render/IndexBuffer.h>

GIGA_CLASS(Singleton)
class GIGA_API RenderSystem : public System {
public:
    RenderSystem();
    virtual ~RenderSystem();
    
    GIGA_CLASS_NAME("RenderSystem");
    
    /**
     * Initialize system
     */
    virtual void Initialize();
    
    /**
     * Update (get render components from scene)
     */
    void Update(float delta);
    
    /**
     * Initialize display
     */
    virtual void Initialize(int width, int height, bool fullscreen) { }
    
    /**
     * Create all of the things
     */
    virtual VertexBuffer* CreateVertexBuffer() { return(new VertexBuffer()); }
    virtual IndexBuffer* CreateIndexBuffer() { return(new IndexBuffer()); }
    virtual VertexFormat* CreateVertexFormat() { return(new VertexFormat()); }
    virtual ShaderProgram* CreateShaderProgram() { return(new ShaderProgram()); }
    virtual Framebuffer* CreateFramebuffer() { return(new Framebuffer()); }
    virtual Texture2D* CreateTexture2D() { return(new Texture2D()); }
    virtual Texture3D* CreateTexture3D() { return(new Texture3D()); }
    
    /**
     * Set/get render pipeline
     */
    void SetRenderPipeline(RenderPipeline* pipeline);
    RenderPipeline* GetRenderPipeline() { return m_renderPipeline; }
    
    /**
     * Clear buffers
     */
    virtual void SetClearColor(vector4 color) { }
    virtual void Clear(int bitmask) { }
    
    /**
     * Draw + indexed draw
     */
    virtual void Draw(int type, int elements) { }
    virtual void DrawIndexed(int type, int elements) { }
    virtual void DrawInstanced(int type, int elements, int instances) { }
    virtual void DrawInstancedIndexed(int type, int elements, int instances) { }
    
    /**
     * Depth testing
     */
    virtual void EnableDepthTest(int type) { }
    virtual void DisableDepthTest() { }
    
    /**
     * Blending
     */
    virtual void EnableBlending() { }
    virtual void DisableBlending() { }
    virtual void SetBlendFunc(int sourceFactor, int destFactor) { }
    
    /**
     * Set viewport (normally done by framebuffer)
     */
    virtual void SetViewport(int width, int height) { }
    virtual void SetViewport(int x, int y, int width, int height) { }
    
    /**
     * Set render type (front/back/none)
     */
    virtual void SetDrawBuffer(int buffer) { }
    
    /**
     * Set/get default framebuffer
     */
    void SetDefaultFramebuffer(int framebuffer) { m_defaultFramebuffer = framebuffer; }
    virtual void UseDefaultFramebuffer() { }
    
    /**
     * Get scene
     */
    Scene* GetScene() { return(m_scene); }
    
    /**
     * Ambient lighting
     */
    vector3 GetAmbientLighting() { return m_ambient; }
    void SetAmbientLighting(vector3 ambient) { m_ambient = ambient; }
    
    /**
     * Enable/disable face culling
     */
    virtual void EnableFaceCulling(int cullmode) { }
    virtual void DisableFaceCulling() { }
    
protected:
    // Window dimensions
    int m_width, m_height;
    
    // Full screen?
    bool m_fullscreen;
    
    // Current render list (scene)
    Scene* m_scene;
    
    // Render pipeline
    RenderPipeline* m_renderPipeline;
    
    // Default framebuffer
    int m_defaultFramebuffer;
    
    // Ambient lighting
    vector3 m_ambient;
    
    // Total rendered triangles
    int m_triCount = 0;
};

#endif
