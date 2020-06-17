#include "newentitydialog.h"
#include "ui_newentitydialog.h"
#include "mainwindow.h"

#include <Core/World.h>

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
    World* world = World::GetInstance();
    Entity* ent = world->CreateEntity();
    ent->name = ui->lineEdit->text().toStdString();

    MainWindow* w = MainWindow::getInstance();
    SceneTreeModel* model = w->GetSceneModel();
    model->addItem(ent);

    QDialog::accept();
}
