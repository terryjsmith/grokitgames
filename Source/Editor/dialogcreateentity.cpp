#include "dialogcreateentity.h"
#include "ui_dialogcreateentity.h"

#include <Core/World.h>
#include <Core/TransformComponent.h>

#include <QMessageBox>

DialogCreateEntity::DialogCreateEntity(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCreateEntity)
{
    ui->setupUi(this);
}

DialogCreateEntity::~DialogCreateEntity()
{
    delete ui;
}

void DialogCreateEntity::accept() {
    std::string name = ui->txtEntityName->text().toStdString();
    if(name.length() == 0) {
        QMessageBox::warning(this, "Error", "Please provide a valid entity name.");
        return;
    }

    World* world = World::GetInstance();
    Entity* entity = world->FindEntity(name);
    if(entity != 0) {
        QMessageBox::warning(this, "Error", "An entity with this name already exists.");
        return;
    }

    entity = new Entity();
    entity->name = name;

    world->AddEntity(entity);

    // Add a TransformComponent
    entity->CreateComponent<TransformComponent>();

    this->hide();
}
