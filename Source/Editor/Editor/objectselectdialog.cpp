#include "objectselectdialog.h"
#include "ui_objectselectdialog.h"
#include "metatypes.h"

#include <QListWidgetItem>

ObjectSelectDialog::ObjectSelectDialog(std::string filter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectSelectDialog)
{
    ui->setupUi(this);
    std::vector<GigaObject*> objects = GigaObject::GetObjects(filter);
    auto it = objects.begin();
    for(; it != objects.end(); it++) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString::fromStdString((*it)->ToString()));
        item->setData(Qt::UserRole, QVariant::fromValue(*it));

        ui->listWidget->addItem(item);
    }
}

ObjectSelectDialog::~ObjectSelectDialog()
{
    delete ui;
}

void ObjectSelectDialog::SetSelectMode(bool multiselect) {
    ui->listWidget->setSelectionMode(multiselect ? QAbstractItemView::MultiSelection : QAbstractItemView::SingleSelection);
}

QList<GigaObject*> ObjectSelectDialog::GetSelectedObjects() {
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    QList<GigaObject*> ret;

    for(int i = 0; i < items.size(); i++) {
        ret.push_back(items[i]->data(Qt::UserRole).value<GigaObject*>());
    }

    return(ret);
}
