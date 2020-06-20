#include "scenetreeview.h"
#include "mainwindow.h"
#include "propertyformbuilder.h"

#include <QLabel>

SceneTreeView::SceneTreeView(QWidget* parent) : QTreeView(parent) {
    initialized = false;
}

void SceneTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    if(initialized == false) return;

    MainWindow* window = MainWindow::getInstance();

    QVBoxLayout* propertyFrame = window->GetPropertyLayout();

    // Clear properties window
    PropertyFormBuilder::clearLayout(propertyFrame);

    SceneTreeModel* model = (SceneTreeModel*)current.model();
    QStandardItem* item = model->itemFromIndex(current);
    Entity* ent = (Entity*)item->data().value<void*>();

    std::vector<Component*> components = ent->GetComponents();
    auto it = components.begin();
    for(; it != components.end(); it++) {
        DataRecord* dr = new DataRecord();
        (*it)->Serialize(dr);

        QFormLayout* layout = PropertyFormBuilder::GetFormLayout(dr, 0);

        QVBoxLayout* newLayout = new QVBoxLayout();
        QLabel* label = new QLabel(QString::fromStdString(((*it)->GetGigaName())));
        label->setStyleSheet("font-weight: bold; background: #888; padding-left: 10px;");
        newLayout->addWidget(label);
        newLayout->addLayout(layout);

        propertyFrame->addLayout(newLayout);
    }
}
