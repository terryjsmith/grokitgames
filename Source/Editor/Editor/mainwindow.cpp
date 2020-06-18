#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newentitydialog.h"
#include "modeltest.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <Render/RenderSystem.h>
#include <Scripting/ScriptingSystem.h>
#include <IO/SQLiteDataLoader.h>
#include <Core/Application.h>

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
    connect(ui->actionEntity, &QAction::triggered, this, &MainWindow::btnCreateEntity_clicked);

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

    SQLiteDataLoader* dataloader =  new SQLiteDataLoader();

    // Look for a game.db file
    QString gamedbFile = directory + "/game.db";
    if(QFile::exists(gamedbFile)) {
        dataloader->Open(gamedbFile.toStdString());
    }

    // Enable menu items
    ui->actionClose_Project->setEnabled(true);
    ui->actionNew_Scene->setEnabled(true);
    ui->actionOpen_Scene->setEnabled(true);
    ui->actionSave_Project->setEnabled(true);
}

void MainWindow::btnCreateEntity_clicked() {
    NewEntityDialog* dlg = new NewEntityDialog(this);
    dlg->setModal(true);
    dlg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
