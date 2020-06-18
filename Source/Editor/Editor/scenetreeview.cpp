#include "scenetreeview.h"
#include "mainwindow.h"

SceneTreeView::SceneTreeView(QWidget* parent) : QTreeView(parent) {

}

/*void SceneTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    //MainWindow* window = MainWindow::getInstance();

    QGroupBox* propertiesWidget = window->GetPropertiesWidget();

    // Clear properties window
    for (auto widget: propertiesWidget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
      delete widget;

    EntityTreeModel* model = (EntityTreeModel*)current.model();
    TreeItem* item = model->getItem(current);
    GigaObject* obj = (GigaObject*)item->GetInternalPointer();

    Component* c = dynamic_cast<Component*>(obj);
    if(c) {
        QFormLayout* layout = window->GetFormLayout(obj->GetGigaName(), obj, 0);
        delete propertiesWidget->layout();
        propertiesWidget->setLayout(layout);
    }
}*/
