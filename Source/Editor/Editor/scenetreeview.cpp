#include "scenetreeview.h"
#include "mainwindow.h"

#include <QLabel>

SceneTreeView::SceneTreeView(QWidget* parent) : QTreeView(parent) {
    initialized = false;
}

void SceneTreeView::ProcessSelectedEntity(Entity* entity) {
    if(initialized == false) return;

    // Make sure the item is selected in the list (if called from external source)
    QItemSelectionModel* selection = this->selectionModel();
    SceneTreeModel* model = (SceneTreeModel*)this->model();
    QStandardItem* item = model->findItem(entity->entityID);
    selection->select(item->index(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    MainWindow* window = MainWindow::getInstance();
    QVBoxLayout* propertyFrame = window->GetPropertyLayout();

    // Clear properties window
    window->clearLayout(propertyFrame);

    Array<Component*> components = entity->GetComponents();
    auto it = components.begin();
    for(; it != components.end(); it++) {
        DataRecord* dr = new DataRecord();
        (*it)->Serialize(dr);

        QFormLayout* layout = window->GetFormLayout(dr, *it, 0);
        layout->setLabelAlignment(Qt::AlignLeft);

        QVBoxLayout* newLayout = new QVBoxLayout();

        QLabel* label = new QLabel(QString::fromStdString(((*it)->GetGigaName())));
        label->setStyleSheet("font-weight: bold; background: #999; padding: 5px; color: #fff;");
        newLayout->addWidget(label);
        newLayout->addLayout(layout);

        propertyFrame->addLayout(newLayout);
    }

    propertyFrame->addStretch(1);
    for(int i = 0; i < (int)components.size(); i++) {
        propertyFrame->setStretch(i, 0);
    }
}

void SceneTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    if(initialized == false) return;

    SceneTreeModel* model = (SceneTreeModel*)current.model();
    QStandardItem* item = model->itemFromIndex(current);
    Entity* ent = (Entity*)item->data().value<void*>();

    ProcessSelectedEntity(ent);
}
