#include "scenetreemodel.h"

SceneTreeModel::SceneTreeModel(QObject *parent) : QStandardItemModel(parent) {

}

QStandardItem* SceneTreeModel::findItem(uint32_t entityID) {
    // Get the entity
    for(int i = 0; i < this->rowCount(); i++) {
        QStandardItem* it = item(i, 0);
        Entity* e = (Entity*)it->data().value<void*>();
        if(e->entityID == entityID) {
            return(it);
        }
    }

    return(0);
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
