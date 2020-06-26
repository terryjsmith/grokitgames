#include "gigaopenglwidget.h"

#include <QTimer>

#include <Core/Application.h>
#include <Render/RenderSystem.h>
#include <Render/Pipelines/DeferredRenderPipeline.h>
#include <IO/Profiler.h>
#include <Core/World.h>

GigaOpenGLWidget::GigaOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    m_renderPipeline = 0;
    m_updateTimer = new Timer();
}

void GigaOpenGLWidget::initializeGL() {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();

    //renderSystem->SetClearColor(vector4(1, 0, 1, 1));

    QOpenGLWidget::initializeGL();
}

void GigaOpenGLWidget::resizeGL(int w, int h) {
    // Get the framebuffer width and height
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &m_framebufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &m_framebufferHeight);

    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->Initialize(m_framebufferWidth, m_framebufferHeight, false);

    if(m_renderPipeline == 0) {
        m_renderPipeline = new DeferredRenderPipeline();
    }

    m_renderPipeline->Initialize(m_framebufferWidth, m_framebufferHeight);

    renderSystem->SetRenderPipeline(m_renderPipeline);

    unsigned int framebuffer = defaultFramebufferObject();
    renderSystem->SetDefaultFramebuffer(framebuffer);

    QOpenGLWidget::resizeGL(w, h);
}

void GigaOpenGLWidget::paintGL() {
    PROFILE_START_FRAME();

    float delta = m_updateTimer->Duration() / 1000.0f;
    m_updateTimer->Reset();

    if(m_framebufferWidth == 0) {
        return;
    }

    Application* app = Application::GetInstance();
    app->Update(delta);

    QOpenGLWidget::paintGL();
    QTimer::singleShot(30, this, SLOT(update()));

    PROFILE_END_FRAME();
}
