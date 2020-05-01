
#include <Render/OpenGL/OpenGLRenderSystem.h>
#include <Render/OpenGL/OpenGL.h>
#include <IO/ResourceSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>
#include <Render/OpenGL/OpenGLTexture2D.h>

void OpenGLRenderSystem::Initialize() {
    RenderSystem::Initialize();
    
    // Set our texture type to OpenGL
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    resourceSystem->RegisterResourceType("Texture2D", "OpenGLTexture2D");
}

void OpenGLRenderSystem::Initialize(int width, int height, bool fullscreen) {
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();

#ifdef WIN32
    if (gl3wInit()) {
        errorSystem->HandleError(new Error(Error::ERROR_FATAL, "Unable to initialize GL3W."));
        return;
    }

    if (!gl3wIsSupported(4, 0)) {
        fprintf(stderr, "OpenGL 4.0 not supported\n");
    }
#endif

    GL_CHECK(glEnable(GL_TEXTURE_2D));
    GL_CHECK(glDisable(GL_CULL_FACE));
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void OpenGLRenderSystem::EnableFaceCulling(int cullmode) {
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glCullFace(cullmode));
}

void OpenGLRenderSystem::DisableFaceCulling() {
    GL_CHECK(glDisable(GL_CULL_FACE));
}

void OpenGLRenderSystem::Clear(int bitmask) {
    GL_CHECK(glClear(bitmask));
}

void OpenGLRenderSystem::SetClearColor(vector4 color) {
    GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
}

void OpenGLRenderSystem::Draw(int type, int elements) {
    GL_CHECK(glDrawArrays(type, 0, elements));
    m_triCount += elements / 3;
}

void OpenGLRenderSystem::DrawIndexed(int type, int elements) {
    GL_CHECK(glDrawElements(type, elements, GL_UNSIGNED_INT, 0));
    m_triCount += elements / 3;
}

void OpenGLRenderSystem::EnableDepthTest(int type) {
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(type));
}

void OpenGLRenderSystem::DisableDepthTest() {
    GL_CHECK(glDisable(GL_DEPTH_TEST));
}

void OpenGLRenderSystem::EnableBlending() {
    GL_CHECK(glEnable(GL_BLEND));
}

void OpenGLRenderSystem::DisableBlending() {
    GL_CHECK(glDisable(GL_BLEND));
}

void OpenGLRenderSystem::SetBlendFunc(int sourceFactor, int destFactor) {
    GL_CHECK(glBlendFunc(sourceFactor, destFactor));
}

void OpenGLRenderSystem::SetViewport(int width, int height) {
    GL_CHECK(glViewport(0, 0, width, height));
}

void OpenGLRenderSystem::SetDrawBuffer(int buffer) {
    GL_CHECK(glDrawBuffer(buffer));
}

void OpenGLRenderSystem::UseDefaultFramebuffer() {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFramebuffer));
}
