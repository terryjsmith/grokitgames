#ifndef SCENETREEVIEW_H
#define SCENETREEVIEW_H

#include <QTreeView>

class SceneTreeView : public QTreeView
{
public:
    SceneTreeView(QWidget* parent = 0);

private slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public:
    bool initialized;
};

#endif // SCENETREEVIEW_H
