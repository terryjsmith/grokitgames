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
    model->addItem(ent);

    // Create transform component and add as child
    TransformComponent* tc = ent->CreateComponent<TransformComponent>();
    model->addChildItem(ent, tc);

    QDialog::accept();
}
