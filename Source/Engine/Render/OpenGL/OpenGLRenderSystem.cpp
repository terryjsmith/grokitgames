
#include <Render/OpenGL/OpenGLRenderSystem.h>
#include <Render/OpenGL/OpenGL.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>
#include <Render/OpenGL/OpenGLTexture2D.h>

void OpenGLRenderSystem::Initialize() {
    RenderSystem::Initialize();
    
    // Set our texture type to OpenGL
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    resourceSystem->RegisterResourceType("Texture2D", "OpenGLTexture2D");
}

void OpenGLRenderSystem::Initialize(int width, int height, bool fullscreen) {
    GL_CHECK(glEnable(GL_TEXTURE_2D));
}

void OpenGLRenderSystem::Clear(int bitmask) {
    GL_CHECK(glClear(bitmask));
}

void OpenGLRenderSystem::SetClearColor(vector4 color) {
    GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
}

void OpenGLRenderSystem::Draw(int type, int elements) {
    GL_CHECK(glDrawArrays(type, 0, elements));
}

void OpenGLRenderSystem::DrawIndexed(int type, int elements) {
    GL_CHECK(glDrawElements(type, elements, GL_UNSIGNED_INT, 0));
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
