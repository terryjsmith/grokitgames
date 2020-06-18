#ifndef SCENETREEVIEW_H
#define SCENETREEVIEW_H

#include <QTreeView>

class SceneTreeView : public QTreeView
{
public:
    SceneTreeView(QWidget* parent = 0);

private slots:
    //void currentChanged(const QModelIndex &current, const QModelIndex &previous);
};

#endif // SCENETREEVIEW_H
