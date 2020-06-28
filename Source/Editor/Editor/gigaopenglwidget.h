#ifndef GIGAOPENGLWIDGET_H
#define GIGAOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <Render/Pipeline.h>
#include <Core/Timer.h>

class GigaOpenGLWidget : public QOpenGLWidget
{
public:
    GigaOpenGLWidget(QWidget *parent = 0);

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

protected:
    int m_framebufferWidth, m_framebufferHeight;
    int m_width, m_height;

    // render pipeline
    Pipeline* m_renderPipeline;

    // Timer for update tick
    Timer* m_updateTimer;

    // Our keyboard array
    std::map<int, bool> m_keys;

    // Camera moving
    float m_moveSpeed, m_rotationSpeed;

    // Target position
    vector3 m_targetPosition;

    // Are we dragging the mouse?
    bool m_dragging;
    bool m_panning;

    // Last mouse position
    QPoint m_lastMousePos;
};

#endif // GIGAOPENGLWIDGET_H
