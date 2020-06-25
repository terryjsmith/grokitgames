#include "newcomponentdialog.h"
#include "ui_newcomponentdialog.h"
#include "mainwindow.h"

#include <Core/MetaSystem.h>
#include <Core/Application.h>
#include <Core/World.h>

NewComponentDialog::NewComponentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewComponentDialog)
{
    ui->setupUi(this);

    // Add entities
    World* world = World::GetInstance();
    auto entities = world->GetEntities();
    auto ei = entities.begin();
    for(; ei != entities.end(); ei++) {
        ui->ddlEntity->addItem((*ei)->name.c_str());
    }

    // Add component types
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    auto componentTypes = metaSystem->GetComponentTypes();

    auto it = componentTypes.begin();
    for(; it != componentTypes.end(); it++) {
        ui->ddlComponentType->addItem((*it).c_str());
    }
}

NewComponentDialog::~NewComponentDialog()
{
    delete ui;
}

void NewComponentDialog::accept() {
    // Get the entity
    World* world = World::GetInstance();
    Entity* entity = world->FindEntity(ui->ddlEntity->currentText().toStdString());

    // Get the model
    SceneTreeModel* model = MainWindow::getInstance()->GetSceneModel();

    // Get the entity
    QStandardItem* item = model->findItem(entity->entityID);

    assert(item != 0);

    // Create the component
    Component* component = entity->CreateComponent(ui->ddlComponentType->currentText().toStdString());
    QStandardItem* newItem = new QStandardItem(ui->ddlComponentType->currentText());
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->appendRow(newItem);

    QDialog::accept();
}
