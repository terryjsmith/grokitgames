#ifndef EDITOROPENGLWIDGET_H
#define EDITOROPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <Render/RenderPipeline.h>
#include <Core/Timer.h>

class EditorOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    EditorOpenGLWidget(QWidget* parent = 0);

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

protected:
    // Framebuffer width and height
    int m_framebufferWidth, m_framebufferHeight;

    // Window width and height
    int m_width, m_height;

    // render pipeline
    RenderPipeline* m_renderPipeline;

    // Timer for update tick
    Timer* m_updateTimer;
};

#endif // EDITOROPENGLWIDGET_H
