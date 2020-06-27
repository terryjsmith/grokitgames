#ifndef SCENETREEVIEW_H
#define SCENETREEVIEW_H

#include <QTreeView>
#include <Core/Entity.h>

class SceneTreeView : public QTreeView
{
public:
    SceneTreeView(QWidget* parent = 0);

    void ProcessSelectedEntity(Entity* entity);

private slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public:
    bool initialized;
};

#endif // SCENETREEVIEW_H
