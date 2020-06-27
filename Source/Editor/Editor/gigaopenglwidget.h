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

protected:
    int m_framebufferWidth, m_framebufferHeight;
    int m_width, m_height;

    // render pipeline
    Pipeline* m_renderPipeline;

    // Timer for update tick
    Timer* m_updateTimer;
};

#endif // GIGAOPENGLWIDGET_H
