#include "gigaopenglwidget.h"
#include "mainwindow.h"

#include <QTimer>
#include <QMouseEvent>
#include <QStatusBar>

#include <Core/Application.h>
#include <Render/RenderSystem.h>
#include <Render/Pipelines/DeferredRenderPipeline.h>
#include <IO/Profiler.h>
#include <Core/World.h>

GigaOpenGLWidget::GigaOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    m_renderPipeline = 0;
    m_updateTimer = new Timer();
    m_moveSpeed = 0.0f;
    m_rotationSpeed = 0.0f;
    m_dragging = false;
    m_panning = false;
}

void GigaOpenGLWidget::initializeGL() {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();

    this->SetEditorMode(EditorMode::EDITORMODE_CAMERA);
    this->SetEditMode(EditMode::EDITMODE_GRAB);

    //renderSystem->SetClearColor(vector4(1, 0, 1, 1));

    QOpenGLWidget::initializeGL();
}

void GigaOpenGLWidget::resizeGL(int w, int h) {
    m_width = w;
    m_height = h;

    this->setMouseTracking(true);

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

bool GigaOpenGLWidget::eventFilter(QObject *obj, QEvent *event)  {
    if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if(keyEvent->key() == Qt::Key_Escape) {
                this->SetEditorMode(EditorMode::EDITORMODE_CAMERA);
            }

            return true;
        } else {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
}

void GigaOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::LeftButton) {
        // Get the index buffer
        RenderSystem* rs = GetSystem<RenderSystem>();
        RenderPipeline* pipeline = rs->GetRenderPipeline();
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

        m_dragging = false;
    }

    if(event->button() == Qt::MouseButton::RightButton) {
        m_panning = false;
    }

    QOpenGLWidget::mouseReleaseEvent(event);
}

void GigaOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::RightButton) {
        m_panning = true;
    }

    if(event->button() == Qt::MouseButton::LeftButton) {
        // Get the depth buffer
        RenderSystem* rs = GetSystem<RenderSystem>();
        RenderPipeline* pipeline = rs->GetRenderPipeline();
        Texture2D* depthTexture = pipeline->GetDepthTexture();

        // Get the position of the mouse click
        QPoint pos = event->pos();

        int scaleFactor = m_framebufferWidth / m_width;

        float* data = (float*)malloc(sizeof(float) * m_framebufferWidth * m_framebufferHeight);
        depthTexture->Bind(0);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
        depthTexture->Unbind();

        // Get the clicked index
        int dindex = (((m_framebufferHeight - (pos.ry() * scaleFactor)) * m_framebufferWidth) + (pos.rx() * scaleFactor));
        float depth = data[dindex];
        free(data);

        // Get camera to determine near and far plane
        CameraComponent* camera = rs->GetScene()->camera;
        float fnear = camera->GetNear();
        float ffar = camera->GetFar();

        matrix4 projMatrixInv = glm::inverse(camera->GetProjectionMatrix());
        matrix4 viewMatrixInv = glm::inverse(camera->GetViewMatrix());

        float z = depth * 2.0 - 1.0;
        vector2 texcoord = vector2(((float)pos.rx() * scaleFactor)/(float)m_framebufferWidth, ((float)pos.ry() * scaleFactor)/(float)m_framebufferHeight);
        vector4 clipSpacePosition = vector4(texcoord.x * 2.0 - 1.0, texcoord.y * 2.0 - 1.0, z, 1.0);
        vector4 viewSpacePosition = projMatrixInv * clipSpacePosition;

        // Perspective division
        viewSpacePosition /= viewSpacePosition.w;

        vector4 worldSpacePosition = viewMatrixInv * viewSpacePosition;
        m_targetPosition = vector3(worldSpacePosition);
        m_dragging = true;
    }

    QOpenGLWidget::mousePressEvent(event);
}

void GigaOpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
    QPoint pos = event->pos();

    QPoint diff = m_lastMousePos - event->pos();
    if(diff.manhattanLength() < 3) {
        //diff = { 0, 0 };
    }

    // Get camera
    RenderSystem* rs = GetSystem<RenderSystem>();
    CameraComponent* camera = rs->GetScene()->camera;

    if(m_panning == true) {
        vector3 up = camera->GetTransform()->GetUp();
        vector3 right = camera->GetTransform()->GetRight();

        float moveFactor = 0.05f;
        camera->GetTransform()->Move(right * (float)-diff.x() * moveFactor);
        camera->GetTransform()->Move(up * (float)-diff.y() * moveFactor);
    }

    if(m_dragging == true) {
        // Figure out distance from camera to target
        float distance = glm::length(camera->GetTransform()->GetWorldPosition() - m_targetPosition);

        // Rotate around X
        float rotateFactor = 0.5f;

        vector3 up = vector3(0, 1, 0);//camera->GetTransform()->GetUp();
        vector3 right = vector3(1, 0, 0);//camera->GetTransform()->GetRight();

        camera->GetTransform()->Rotate(right, (float)-diff.y() * rotateFactor);
        camera->GetTransform()->Rotate(up, (float)-diff.x() * rotateFactor);

        vector3 look = glm::normalize(camera->GetTransform()->GetLook());
        vector3 position = m_targetPosition - (distance * look);
        camera->GetTransform()->SetWorldPosition(position);

        float checkDistance = glm::length(camera->GetTransform()->GetWorldPosition() - m_targetPosition);
    }

    m_lastMousePos = pos;
    QOpenGLWidget::mouseMoveEvent(event);
}

void GigaOpenGLWidget::wheelEvent(QWheelEvent* event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    CameraComponent* camera = renderSystem->GetScene()->camera;
    if (!numPixels.isNull()) {
        vector3 look = camera->GetTransform()->GetLook();
        camera->GetTransform()->Move(look * (float)numPixels.y() * 0.1f);
    } else if (!numDegrees.isNull()) {
    }

    event->accept();
    QOpenGLWidget::wheelEvent(event);
}

void GigaOpenGLWidget::SetEditorMode(EditorMode mode) {
    QStatusBar* bar = MainWindow::getInstance()->statusBar();
    switch(mode) {
    case EditorMode::EDITORMODE_CAMERA:
        bar->showMessage("Editor mode: camera");
        break;
    case EditorMode::EDITORMODE_OBJECT:
        bar->showMessage("Editor mode: object");
        break;
    default: break;
    };
}
