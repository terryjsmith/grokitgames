#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newentitydialog.h"
#include "newcomponentdialog.h"
#include "modeltest.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <Render/RenderSystem.h>
#include <Scripting/ScriptingSystem.h>
#include <IO/SQLiteDataLoader.h>
#include <Core/Application.h>
#include <Core/World.h>

MainWindow* MainWindow::m_instance = 0;

MainWindow* MainWindow::getInstance() {
    if(m_instance == 0) {
        m_instance = new MainWindow();
    }

    return(m_instance);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    connect(ui->actionOpen_Project, &QAction::triggered, this, &MainWindow::btnOpenProject_clicked);
    connect(ui->actionSave_Project, &QAction::triggered, this, &MainWindow::btnSaveProject_clicked);
    connect(ui->actionEntity, &QAction::triggered, this, &MainWindow::btnCreateEntity_clicked);
    connect(ui->actionComponent, &QAction::triggered, this, &MainWindow::btnCreateComponent_clicked);

    ui->tabWidget_3->removeTab(1);

    // Initialize tree model
    m_sceneTreeModel = new SceneTreeModel(0);
    ui->sceneView->setModel(m_sceneTreeModel);

    ModelTest test(m_sceneTreeModel);
}

void MainWindow::btnOpenProject_clicked() {
    // Have the user choose a project directory
    QString filePath = QFileDialog::getOpenFileName(this, "Open Project", QDir::currentPath(), "Eternity Project DC (*.gpf)");
    if(filePath.length() == 0) {
        return;
    }

    QString path = filePath;
    QString directory = path = path.mid(0, path.lastIndexOf("/"));

    QFile projFile(filePath);
    if(!projFile.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open project settings file.");
        return;
    }

    QByteArray jsonData = projFile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    QJsonObject settingsObject = jsonDoc.object();

    setWindowTitle("GIGA Editor - " + settingsObject["name"].toString());

    //m_currentProjectDirectory = directory;
    QDir::setCurrent(directory);

    // Load a game.dll file if one exists
    QString gameDllFile = directory + "/game.dll";
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();

    if(QFile::exists(gameDllFile)) {
        scriptingSystem->LoadLibrary(gameDllFile.toStdString());
    }

    m_dataLoader =  new SQLiteDataLoader();

    // Look for a game.db file
    QString gamedbFile = directory + "/game.db";
    m_dataLoader->Open(gamedbFile.toStdString());
    if(QFile::exists(gamedbFile)) {
        World* world = World::GetInstance();
        MetaSystem* metaSystem = GetSystem<MetaSystem>();

        // Load entities
        std::vector<GigaObject*> entities = m_dataLoader->GetObjects("Entity");

        // Add entities to world
        auto it = entities.begin();
        for(; it != entities.end(); it++) {
            Entity* entity = dynamic_cast<Entity*>(*it);
            world->AddEntity(entity);
            QStandardItem* item = new QStandardItem(entity->name.c_str());
            QVariant value = qVariantFromValue((void*)entity);
            item->setData(value);
            m_sceneTreeModel->appendRow(item);
        }

        // Load components
        std::vector<std::string> componentTypes = metaSystem->GetComponentTypes();
        auto cti = componentTypes.begin();
        for(; cti != componentTypes.end(); cti++) {
            std::vector<GigaObject*> components = m_dataLoader->GetObjects(*cti);
            auto ci = components.begin();
            for(; ci != components.end(); ci++) {
                // Check whether this is attached to an entity
                Component* component = dynamic_cast<Component*>(*ci);
                Entity* entity = component->GetParent();
                if(entity != 0) {
                    QStandardItem* item = m_sceneTreeModel->findItem(entity);

                    QStandardItem* newItem = new QStandardItem((*cti).c_str());
                    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    item->appendRow(newItem);
                }
            }
        }
    }

    // Enable menu items
    ui->actionClose_Project->setEnabled(true);
    ui->actionNew_Scene->setEnabled(true);
    ui->actionOpen_Scene->setEnabled(true);
    ui->actionSave_Project->setEnabled(true);
}

void MainWindow::btnSaveProject_clicked() {
    World* world = World::GetInstance();
    MetaSystem* metaSystem = GetSystem<MetaSystem>();

    // Save entities
    std::vector<Entity*> entities = world->GetEntities();

    std::vector<GigaObject*> objs;
    auto it = entities.begin();
    for(; it != entities.end(); it++) {
        objs.push_back(*it);
    }

    m_dataLoader->SaveObjects("Entity", objs);
    objs.clear();

    // Save components
    std::vector<std::string> componentTypes = metaSystem->GetComponentTypes();
    auto cti = componentTypes.begin();
    for(; cti != componentTypes.end(); cti++) {
        std::vector<Component*> components = world->FindComponents(*cti);

        auto it = components.begin();
        for(; it != components.end(); it++) {
            objs.push_back(*it);
        }

        m_dataLoader->SaveObjects(*cti, objs);

        objs.clear();
    }
}

void MainWindow::btnCreateEntity_clicked() {
    NewEntityDialog* dlg = new NewEntityDialog(this);
    dlg->setModal(true);
    dlg->show();
}

void MainWindow::btnCreateComponent_clicked() {
    NewComponentDialog* dlg = new NewComponentDialog(this);
    dlg->setModal(true);
    dlg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
