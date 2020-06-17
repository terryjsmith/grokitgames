#ifndef SCENETREEMODEL_H
#define SCENETREEMODEL_H

#include "treemodel.h"
#include <Core/Entity.h>
#include <Core/Component.h>

class SceneTreeModel : public TreeModel
{
public:
    SceneTreeModel(QObject *parent);

    void addItem(Entity* entity);
    void addChildItem(Entity* entity, Component* component);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    int getRowNumber(Entity* entity);
};

#endif // SCENETREEMODEL_H
