#ifndef GIGAOPENGLWIDGET_H
#define GIGAOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <Render/RenderPipeline.h>
#include <Core/Timer.h>

#include "translationgizmocomponent.h"
#include "scalegizmocomponent.h"
#include "rotationgizmocomponent.h"

class GigaOpenGLWidget : public QOpenGLWidget
{
public:
    GigaOpenGLWidget(QWidget *parent = 0);

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    enum EditorMode {
        EDITORMODE_CAMERA = 1,
        EDITORMODE_OBJECT
    };

    enum EditMode {
        EDITMODE_GRAB = 1,
        EDITMODE_MOVE,
        EDITMODE_ROTATE,
        EDITMODE_SCALE
    };

    enum EditAxis {
        EDITAXIS_NONE = 0,
        EDITAXIS_X = 1,
        EDITAXIS_Y,
        EDITAXIS_Z
    };

    void SetEditorMode(EditorMode mode);
    EditorMode GetEditorMode() { return m_editorMode; }

    void SetEditMode(EditMode mode);
    EditMode GetEditMode() { return m_editMode; }

    void SetSelectedEntity(Entity* entity);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    int m_framebufferWidth, m_framebufferHeight;
    int m_width, m_height;

    // render pipeline
    RenderPipeline* m_renderPipeline;

    // Timer for update tick
    Timer* m_updateTimer;

    // Our keyboard array
    std::map<int, bool> m_keys;

    // Camera moving
    float m_moveSpeed, m_rotationSpeed;

    // Target position
    vector3 m_targetPosition;
    float m_targetDistance;

    // Our current editor mode
    EditorMode m_editorMode;
    EditMode m_editMode;
    EditAxis m_editAxis;

    // Are we dragging the mouse?
    bool m_dragging;
    bool m_panning;

    // Selected entity
    Entity* m_selectedEntity;

    // Last mouse position
    QPoint m_lastMousePos;

    // Gizmos
    TranslationGizmoComponent* m_translationComponent;
    ScaleGizmoComponent* m_scaleComponent;
    RotationGizmoComponent* m_rotationComponent;
};

#endif // GIGAOPENGLWIDGET_H
