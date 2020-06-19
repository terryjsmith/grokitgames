#include "newentitydialog.h"
#include "ui_newentitydialog.h"
#include "mainwindow.h"

#include <Core/World.h>
#include <Core/TransformComponent.h>

NewEntityDialog::NewEntityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewEntityDialog)
{
    ui->setupUi(this);
}

NewEntityDialog::~NewEntityDialog()
{
    delete ui;
}

void NewEntityDialog::accept() {
    // Create a new entity
    World* world = World::GetInstance();
    Entity* ent = world->CreateEntity();
    ent->name = ui->lineEdit->text().toStdString();

    MainWindow* w = MainWindow::getInstance();
    SceneTreeModel* model = w->GetSceneModel();

    QStandardItem* item = new QStandardItem(ent->name.c_str());
    QVariant value = qVariantFromValue((void*)ent);
    item->setData(value);
    model->appendRow(item);

    // Create transform component and add as child
    TransformComponent* tc = ent->CreateComponent<TransformComponent>();
    QStandardItem* child = new QStandardItem(tc->GetGigaName().c_str());
    child->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->appendRow(child);

    QDialog::accept();
}
