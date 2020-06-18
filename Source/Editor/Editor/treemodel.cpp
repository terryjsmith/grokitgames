#include "treemodel.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return m_rootItem->data(section);

        return QVariant();
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

        TreeItem *parentItem;

        if (!parent.isValid())
            parentItem = m_rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        TreeItem *childItem = parentItem->child(row);
        if (childItem)
            return createIndex(row, column, childItem);
        else
            return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
            return QModelIndex();

        TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
        TreeItem *parentItem = childItem->parentItem();

        if (parentItem == m_rootItem)
            return QModelIndex();

        return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    return m_rootItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return m_rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

        if (role != Qt::DisplayRole)
            return QVariant();

        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

        return item->data(index.column());
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }

    return m_rootItem;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        if (role != Qt::EditRole)
                return false;

            TreeItem *item = getItem(index);
            bool result = item->setData(index.column(), value);

            if (result)
                emit dataChanged(index, index);

            return result;
    }
    return false;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
            return Qt::ItemIsDropEnabled;
        }

        Qt::ItemFlags flags = Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if(item->GetEditable()) {
            flags |= Qt::ItemIsEditable;
        }

        return flags;
}

void TreeModel::empty() {
    if(m_rootItem == 0) {
        return;
    }

    int childCount = m_rootItem->childCount() ;
    if(childCount) {
        beginRemoveRows(QModelIndex(), 0, childCount - 1);
        for(int i = childCount - 1; i >= 0; i--) {
            m_rootItem->removeChild(i);
        }
        endRemoveRows();
    }
}

void TreeModel::removeRow(int row) {
    if(m_rootItem == 0) {
        return;
    }

    int childCount = m_rootItem->childCount();
    if(childCount) {
        beginRemoveRows(QModelIndex(), row, row + 1);
        m_rootItem->removeChild(row);
        endRemoveRows();
    }
}
