#include "scenetreemodel.h"

SceneTreeModel::SceneTreeModel(QObject *parent) : QStandardItemModel(parent) {
    QList<QVariant> rootData;
    rootData << "Entities";

    QStandardItem *parentItem = invisibleRootItem();
}

QStandardItem* SceneTreeModel::findItem(Entity* entity) {
    // Get the entity
    QStandardItem* item = 0;
    QList<QStandardItem* > items = findItems(entity->name.c_str());
    for(int i = 0; i < items.size(); i++) {
        Entity* e = (Entity*)items[i]->data().value<void*>();
        if(e == entity) {
            item = items[i];
            break;
        }
    }

    return(item);
}

bool SceneTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    std::string name = value.toString().toStdString();
    if(name.length() == 0) {
        return(false);
    }

    // Get the item
    QStandardItem* item = itemFromIndex(index);

    // Get the entity
    Entity* entity = (Entity*)item->data().value<void*>();
    entity->name = name;

    QStandardItemModel::setData(index, value, role);
    return(true);
}
