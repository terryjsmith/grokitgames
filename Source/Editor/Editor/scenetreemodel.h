#ifndef SCENETREEMODEL_H
#define SCENETREEMODEL_H

#include <QStandardItemModel>

#include <Core/Entity.h>
#include <Core/Component.h>

class SceneTreeModel : public QStandardItemModel
{
public:
    SceneTreeModel(QObject *parent);

    // Find an entity
    QStandardItem* findItem(Entity* entity);

    // Override setData to update entity names
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};

#endif // SCENETREEMODEL_H
