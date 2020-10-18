#include "newobjectdialog.h"
#include "ui_newobjectdialog.h"
#include "mainwindow.h"

#include <Core/MetaSystem.h>
#include <Core/Application.h>

NewObjectDialog::NewObjectDialog(QString type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewObjectDialog)
{
    ui->setupUi(this);

    // Create a blank object
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Meta::Class* cl = metaSystem->FindClass(type.toStdString());
    GIGA_ASSERT(cl != 0, "Class type not found.");

    GigaObject* obj = cl->CreateObject();

    // Create a new data record
    DataRecord* record = new DataRecord();

    // Serialize
    obj->SerializeObject(record);

    // Get form layout
    MainWindow* mainWindow = MainWindow::getInstance();
    QGridLayout* formLayout = new QGridLayout();
    mainWindow->GetFormLayout(record, obj, this, formLayout, QString::fromStdString(obj->GetGigaName()));

    ui->verticalLayout->addLayout(formLayout);
}

NewObjectDialog::~NewObjectDialog()
{
    delete ui;
}
