#include "editoropenglwidget.h"
#include <QTimer>

#include <Render/RenderSystem.h>
#include <Render/Pipelines/DeferredRenderPipeline.h>
#include <Core/Application.h>
#include <IO/Profiler.h>

EditorOpenGLWidget::EditorOpenGLWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    m_renderPipeline = 0;
    m_updateTimer = new Timer();
}

void EditorOpenGLWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();
    QOpenGLFunctions::initializeOpenGLFunctions();
}

void EditorOpenGLWidget::resizeGL(int w, int h)
{
    m_width = w;
    m_height = h;

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

void EditorOpenGLWidget::paintGL()
{
    PROFILE_START_FRAME();

    float delta = m_updateTimer->Duration() / 1000.0f;
    m_updateTimer->Reset();

    Application* app = Application::GetInstance();
    app->Update(delta);

    QOpenGLWidget::paintGL();
    QTimer::singleShot(30, this, SLOT(update()));

    PROFILE_END_FRAME();
}
