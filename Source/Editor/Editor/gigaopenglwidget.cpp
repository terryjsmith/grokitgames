#include "gigaopenglwidget.h"
#include "mainwindow.h"

#include <QTimer>
#include <QMouseEvent>

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

void GigaOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::LeftButton) {
        // Get the index buffer
        RenderSystem* rs = GetSystem<RenderSystem>();
        Pipeline* pipeline = rs->GetRenderPipeline();
        Texture2D* indexTexture = pipeline->GetIndexTexture();

        // Get the position of the mouse click
        QPoint pos = event->pos();

        int scaleFactor = m_framebufferWidth / m_width;

        float* data = (float*)malloc(sizeof(float) * m_framebufferWidth * m_framebufferHeight * 3);
        indexTexture->Bind(0);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, data);
        indexTexture->Unbind();

        // Get the clicked index
        int dindex = (((m_framebufferHeight - (pos.ry() * scaleFactor)) * m_framebufferWidth) + (pos.rx() * scaleFactor)) * 3;
        float index = data[dindex];
        free(data);

        // Index is 1-based, so need to subtract 1 to see if valid
        index = index - 1;
        if(index >= 0) {
            // Get clicked item
            RenderComponent* rc = rs->GetScene()->renderables[index];

            MainWindow* mainWindow = MainWindow::getInstance();

            // Select item
            SceneTreeView* view = mainWindow->GetSceneView();
            view->ProcessSelectedEntity(rc->GetParent());
        }
    }

    QOpenGLWidget::mouseReleaseEvent(event);
}

void GigaOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    QOpenGLWidget::mousePressEvent(event);
}

void GigaOpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
    QOpenGLWidget::mouseMoveEvent(event);
}

void GigaOpenGLWidget::wheelEvent(QWheelEvent* event) {
    QOpenGLWidget::wheelEvent(event);
}
