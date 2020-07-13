#include "gigaopenglwidget.h"
#include "mainwindow.h"

#include <QTimer>
#include <QMouseEvent>
#include <QStatusBar>

#include <Core/TransformComponent.h>
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
    m_targetDistance = 0;
    m_selectedEntity = 0;
    m_translationComponent = 0;
    m_scaleComponent = 0;
    m_rotationComponent = 0;
    m_editAxis = EDITAXIS_NONE;
    m_editorMode = EDITORMODE_CAMERA;
}

void GigaOpenGLWidget::initializeGL() {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    World* world = World::GetInstance();

    m_translationComponent =  new TranslationGizmoComponent();
    m_scaleComponent =  new ScaleGizmoComponent();
    m_rotationComponent =  new RotationGizmoComponent();

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
            MainWindow* mainWindow = MainWindow::getInstance();
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            if(keyEvent->key() == Qt::Key_Escape) {
                this->SetEditorMode(EditorMode::EDITORMODE_CAMERA);
                this->SetSelectedEntity(0);
                m_editAxis = EDITAXIS_NONE;
            }

            if(keyEvent->key() == Qt::Key_G && m_keys[Qt::Key_Alt] == true) {
                mainWindow->SetEditMode("Grab");
                m_editMode = EditMode::EDITMODE_GRAB;
            }

            if(keyEvent->key() == Qt::Key_R && m_keys[Qt::Key_Alt] == true) {
                mainWindow->SetEditMode("Rotate");
                m_editMode = EditMode::EDITMODE_ROTATE;
            }

            if(keyEvent->key() == Qt::Key_M && m_keys[Qt::Key_Alt] == true) {
                mainWindow->SetEditMode("Move");
                m_editMode = EditMode::EDITMODE_MOVE;
            }

            if(keyEvent->key() == Qt::Key_S && m_keys[Qt::Key_Alt] == true) {
                mainWindow->SetEditMode("Scale");
                m_editMode = EditMode::EDITMODE_SCALE;
            }

            m_keys[keyEvent->key()] = true;
            return true;
    }

    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        m_keys[keyEvent->key()] = false;

        return true;
    }

    return QObject::eventFilter(obj, event);
}

void GigaOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::LeftButton) {
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
        Texture2D* indexTexture = pipeline->GetIndexTexture();

        // Get the position of the mouse click
        QPoint pos = event->pos();

        int scaleFactor = m_framebufferWidth / m_width;

        float* data = (float*)malloc(sizeof(float) * m_framebufferWidth * m_framebufferHeight);
        depthTexture->Bind(0);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
        depthTexture->Unbind();

        // Get the clicked index
        vector2 screenCenter = vector2(m_framebufferWidth / 2.0f, m_framebufferHeight / 2.0f);
        int dindex = (screenCenter.y * m_framebufferWidth) + screenCenter.x;
        float depth = data[dindex];
        free(data);

        // Get camera to determine near and far plane
        CameraComponent* camera = rs->GetScene()->camera;
        matrix4 projMatrix = camera->GetProjectionMatrix();
        matrix4 viewMatrix = camera->GetViewMatrix();
        vector4 viewport = vector4(0, 0, m_framebufferWidth, m_framebufferHeight);

        vector3 objectCoords = glm::unProject(vector3(screenCenter.x, screenCenter.y, depth), viewMatrix, projMatrix, viewport);

        m_targetPosition = objectCoords;
        vector3 distanceVec = camera->GetTransform()->GetWorldPosition() - m_targetPosition;
        m_targetDistance = glm::length(distanceVec);
        m_dragging = true;
        m_editorMode = EDITORMODE_OBJECT;

        data = (float*)malloc(sizeof(float) * m_framebufferWidth * m_framebufferHeight * 4);
        indexTexture->Bind(0);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
        indexTexture->Unbind();

        // Get the clicked index
        dindex = (((m_framebufferHeight - (pos.ry() * scaleFactor)) * m_framebufferWidth) + (pos.rx() * scaleFactor)) * 4;
        float index = data[dindex];
        float childIndex = data[dindex + 3];
        free(data);

        // Actual child index (1-based as well)
        childIndex--;

        // Index is 1-based, so need to subtract 1 to see if valid
        index = index - 1;
        if(index >= 0) {
            // Get clicked item
            RenderComponent* rc = rs->GetScene()->renderables[index];
            m_selectedEntity = rc->GetParent();

            MainWindow* mainWindow = MainWindow::getInstance();

            // Select item
            SceneTreeView* view = mainWindow->GetSceneView();
            view->ProcessSelectedEntity(rc->GetParent());

            // Reset edit axis
            m_editAxis = EDITAXIS_NONE;

            // Check if we clicked a "special" object
            TranslationGizmoComponent* tgc = dynamic_cast<TranslationGizmoComponent*>(rc);
            QStatusBar* bar = MainWindow::getInstance()->statusBar();
            if(tgc) {
                // Set axis to which one was clicked (0 = Y, 1 = Z, 2 = X)
                switch((int)childIndex) {
                case 0:
                    m_editAxis = EDITAXIS_Y;
                    bar->showMessage("Edit axis: Y");
                    break;
                case 1:
                    m_editAxis = EDITAXIS_Z;
                    bar->showMessage("Edit axis: Z");
                    break;
                case 2:
                    m_editAxis = EDITAXIS_X;
                    bar->showMessage("Edit axis: X");
                    break;
                default: break;
                };
            }

            RotationGizmoComponent* rgc = dynamic_cast<RotationGizmoComponent*>(rc);
            if(rgc) {
                // Set axis to which one was clicked (0 = X, 1 = Z, 2 = Y)
                switch((int)childIndex) {
                case 0:
                    m_editAxis = EDITAXIS_X;
                    bar->showMessage("Edit axis: X");
                    break;
                case 1:
                    m_editAxis = EDITAXIS_Z;
                    bar->showMessage("Edit axis: Z");
                    break;
                case 2:
                    m_editAxis = EDITAXIS_Y;
                    bar->showMessage("Edit axis: Y");
                    break;
                default: break;
                };
            }

            ScaleGizmoComponent* sgc = dynamic_cast<ScaleGizmoComponent*>(rc);
            if(sgc) {
                // Set axis to which one was clicked (0 = Y, 1 = Z, 2 = X)
                switch((int)childIndex) {
                case 0:
                    m_editAxis = EDITAXIS_Y;
                    bar->showMessage("Edit axis: Y");
                    break;
                case 1:
                    m_editAxis = EDITAXIS_Z;
                    bar->showMessage("Edit axis: Z");
                    break;
                case 2:
                    m_editAxis = EDITAXIS_X;
                    bar->showMessage("Edit axis: X");
                    break;
                default: break;
                };
            }
        }
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
        // If no edit axis, rotate around object
        if(m_editAxis == EDITAXIS_NONE) {
            // Rotate around X
            float rotateFactor = 0.5f;

            vector3 up = vector3(0, 1, 0);//camera->GetTransform()->GetUp();
            vector3 right = vector3(1, 0, 0);//camera->GetTransform()->GetRight();

            camera->GetTransform()->Rotate(right, (float)diff.y() * rotateFactor);
            camera->GetTransform()->Rotate(up, (float)-diff.x() * rotateFactor);

            vector3 look = glm::normalize(camera->GetTransform()->GetLook());
            vector3 position = m_targetPosition - (m_targetDistance * look);
            camera->GetTransform()->SetWorldPosition(position);
        }

        if(m_editAxis && m_editorMode == EDITORMODE_OBJECT && m_editMode == EDITMODE_MOVE) {
            float moveFactor = 0.1f;
            TransformComponent* tc = m_selectedEntity->GetComponent<TransformComponent>();
            float fdiff = m_editAxis == EDITAXIS_Y ? (float)diff.y() : (float)diff.x();
            vector3 amount = vector3(fdiff * moveFactor);

            // Mask based on selected axis
            amount.x = amount.x * (m_editAxis == EDITAXIS_X ? 1.0f : 0.0f);
            amount.y = amount.y * (m_editAxis == EDITAXIS_Y ? 1.0f : 0.0f);
            amount.z = amount.z * (m_editAxis == EDITAXIS_Z ? 1.0f : 0.0f);

            tc->GetTransform()->Move(amount);
        }

        if(m_editAxis && m_editorMode == EDITORMODE_OBJECT && m_editMode == EDITMODE_SCALE) {
            float moveFactor = 0.1f;
            TransformComponent* tc = m_selectedEntity->GetComponent<TransformComponent>();
            float fdiff = m_editAxis == EDITAXIS_Y ? (float)diff.y() : (float)diff.x();
            vector3 amount = vector3(fdiff * moveFactor);

            // Mask based on selected axis
            amount.x = amount.x * (m_editAxis == EDITAXIS_X ? 1.0f : 0.0f);
            amount.y = amount.y * (m_editAxis == EDITAXIS_Y ? 1.0f : 0.0f);
            amount.z = amount.z * (m_editAxis == EDITAXIS_Z ? 1.0f : 0.0f);

            tc->GetTransform()->Scale(amount);
        }

        if(m_editAxis && m_editorMode == EDITORMODE_OBJECT && m_editMode == EDITMODE_ROTATE) {
            float moveFactor = 0.1f;
            TransformComponent* tc = m_selectedEntity->GetComponent<TransformComponent>();
            float fdiff = m_editAxis == EDITAXIS_Y ? (float)diff.y() : (float)diff.x();
            vector3 amount = vector3(fdiff * moveFactor);

            // Mask based on selected axis
            amount.x = amount.x * (m_editAxis == EDITAXIS_X ? 1.0f : 0.0f);
            amount.y = amount.y * (m_editAxis == EDITAXIS_Y ? 1.0f : 0.0f);
            amount.z = amount.z * (m_editAxis == EDITAXIS_Z ? 1.0f : 0.0f);

            tc->GetTransform()->Move(amount);
        }
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

void GigaOpenGLWidget::SetEditMode(EditMode mode) {
    m_editMode = mode;
    World* world = World::GetInstance();

    if(m_selectedEntity) {
        m_selectedEntity->RemoveComponent(m_scaleComponent);
        m_selectedEntity->RemoveComponent(m_rotationComponent);
        m_selectedEntity->RemoveComponent(m_translationComponent);

        MeshComponent* rc = m_selectedEntity->GetComponent<MeshComponent>();
        vector3 center = vector3(0.0f);
        if(rc) {
            vector3 center = rc->GetBoundingSphere()->center; // In world position
            center -= rc->GetTransform()->GetWorldPosition();
        }

        if(m_editMode == EDITMODE_MOVE) {
            m_translationComponent->GetTransform()->SetLocalPosition(center);
            m_selectedEntity->AddComponent(m_translationComponent);
        }

        if(m_editMode == EDITMODE_ROTATE) {
            m_rotationComponent->GetTransform()->SetLocalPosition(center);
            m_selectedEntity->AddComponent(m_rotationComponent);
        }

        if(m_editMode == EDITMODE_SCALE) {
            m_scaleComponent->GetTransform()->SetLocalPosition(center);
            m_selectedEntity->AddComponent(m_scaleComponent);
        }
    }
}

void GigaOpenGLWidget::SetSelectedEntity(Entity* entity) {
    World* world = World::GetInstance();

    if(entity == 0 && m_selectedEntity) {
        m_selectedEntity->RemoveComponent(m_scaleComponent);
        m_selectedEntity->RemoveComponent(m_rotationComponent);
        m_selectedEntity->RemoveComponent(m_translationComponent);
        m_selectedEntity = 0;
        return;
    }

    m_selectedEntity = entity;

    MeshComponent* rc = m_selectedEntity->GetComponent<MeshComponent>();
    vector3 center = vector3(0.0f);
    if(rc) {
        vector3 center = rc->GetBoundingSphere()->center; // In world position
        center -= rc->GetTransform()->GetWorldPosition();
    }

    if(m_editMode == EDITMODE_MOVE) {
        m_translationComponent->GetTransform()->SetLocalPosition(center);
        m_selectedEntity->AddComponent(m_translationComponent);
    }

    if(m_editMode == EDITMODE_ROTATE) {
        m_rotationComponent->GetTransform()->SetLocalPosition(center);
        m_selectedEntity->AddComponent(m_rotationComponent);
    }

    if(m_editMode == EDITMODE_SCALE) {
        m_scaleComponent->GetTransform()->SetLocalPosition(center);
        m_selectedEntity->AddComponent(m_scaleComponent);
    }
}
