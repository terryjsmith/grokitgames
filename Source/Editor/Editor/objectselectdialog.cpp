#include "objectselectdialog.h"
#include "ui_objectselectdialog.h"
#include "metatypes.h"
#include "newobjectdialog.h"

#include <QListWidgetItem>
#include <QPushButton>

ObjectSelectDialog::ObjectSelectDialog(std::string filter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectSelectDialog)
{
    m_filter = filter;
    ui->setupUi(this);
    Array<GigaObject*> objects = GigaObject::GetObjects(filter);
    auto it = objects.begin();
    for(; it != objects.end(); it++) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setText(QString::fromStdString((*it)->ToString()));
        item->setData(Qt::UserRole, QVariant::fromValue(*it));

        ui->listWidget->addItem(item);
    }

    QPushButton* newButton = ui->buttonBox->addButton("New...", QDialogButtonBox::ActionRole);
    connect(newButton, &QPushButton::clicked, this, &ObjectSelectDialog::btnNewObject_clicked);
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

void ObjectSelectDialog::btnNewObject_clicked() {
    NewObjectDialog* dlg = new NewObjectDialog(QString::fromStdString(m_filter), this);
    dlg->setModal(true);
    int returnCode = dlg->exec();
}
