#include "scenetreeview.h"
#include "mainwindow.h"
#include "gigaopenglwidget.h"

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

    QGridLayout* layout = new QGridLayout();

    Array<Component*> components = entity->GetComponents();
    auto it = components.begin();
    for(; it != components.end(); it++) {
        DataRecord* dr = new DataRecord();
        (*it)->Serialize(dr);

        window->GetFormLayout(dr, *it, 0, layout, QString::fromStdString((*it)->GetGigaName()));

        /*QLabel* label = new QLabel(QString::fromStdString(((*it)->GetGigaName())));
        label->setStyleSheet("font-weight: bold; background: #999; padding: 5px; color: #fff;");
        newLayout->addWidget(label);
        newLayout->addLayout(layout);

        propertyFrame->addLayout(newLayout);*/
    }

    propertyFrame->addLayout(layout);

    propertyFrame->addStretch(1);
    propertyFrame->setStretch(0, 0);

    GigaOpenGLWidget* widget = MainWindow::getInstance()->GetOpenGLWidget();
    widget->SetEditorMode(GigaOpenGLWidget::EditorMode::EDITORMODE_OBJECT);
    widget->SetSelectedEntity(entity);
}

void SceneTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    if(initialized == false) return;

    SceneTreeModel* model = (SceneTreeModel*)current.model();
    QStandardItem* item = model->itemFromIndex(current);
    Entity* ent = (Entity*)item->data().value<void*>();

    ProcessSelectedEntity(ent);
}
