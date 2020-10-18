#ifndef EDITORMODE_H
#define EDITORMODE_H

#include <QMouseEvent>
#include <Core/Entity.h>
#include <Render/MeshComponent.h>

class EditorMode
{
public:
    EditorMode();
    virtual ~EditorMode() { }

    void SetSelectedEntity(Entity* entity) { m_selectedEntity = entity; }
    void SetSelectedMesh(MeshComponent* mesh) { m_selectedMesh = mesh; }

    virtual void mousePressEvent(QMouseEvent *event) { }
    virtual void mouseReleaseEvent(QMouseEvent *event) { }
    virtual void mouseMoveEvent(QMouseEvent* event) { }
    virtual void wheelEvent(QWheelEvent* event) { }

protected:
    MeshComponent* m_selectedMesh;
    Entity* m_selectedEntity;
};

#endif // EDITORMODE_H
