#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogcreateentity.h"
#include "metatypes.h"

#include <Core/MessageSystem.h>
#include <Core/Application.h>

#include <QFileSystemModel>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

MainWindow* MainWindow::m_instance = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Remove the close (X) icon from the first viewport tab
    ui->mainTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);

    // Load the folder
    QFileSystemModel* model = new QFileSystemModel(this);
    model->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    model->setRootPath("C:/Users/Smith/Documents");
    ui->listViewResources->setModel(model);
    ui->listViewResources->setRootIndex(model->setRootPath("C:/Users/Smith/Documents"));

    QFileIconProvider* provider = new QFileIconProvider();
    model->setIconProvider(provider);

    // Set up the model and proxy model to allow filtering the scene
    QStandardItemModel* sceneModel = new QStandardItemModel(this);
    QSortFilterProxyModel* filterModel = new QSortFilterProxyModel(this);

    filterModel->setSourceModel(sceneModel);
    ui->viewScene->setModel(filterModel);

    // Connect the signals
    connect(sceneModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotSceneItemChanged(QStandardItem*)));
    connect(ui->btnCreateEntity, SIGNAL(clicked()), this, SLOT(slotCreateEntityClicked()));
    connect(ui->txtSearchScene, SIGNAL(textChanged(QString)), this, SLOT(slotSceneFilter(QString)));
    connect(ui->viewScene, SIGNAL(clicked(QModelIndex)), this, SLOT(sceneItemClicked(QModelIndex)));

    // Register for callbacks
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    messageSystem->RegisterCallback(0, "ENTITY_ADDED", MainWindow::UpdateSceneTree);
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow* MainWindow::GetInstance() {
    if(m_instance == 0) {
        m_instance = new MainWindow();
    }

    return(m_instance);
}

void MainWindow::slotCreateEntityClicked() {
    DialogCreateEntity* dlg = new DialogCreateEntity();
    dlg->show();
}

void MainWindow::UpdateSceneTree(GigaObject* obj, Message* message) {
    // Get list view
    MainWindow* window = GetInstance();
    QSortFilterProxyModel* proxyModel = (QSortFilterProxyModel*)window->ui->viewScene->model();
    QStandardItemModel* sceneModel = (QStandardItemModel*)proxyModel->sourceModel();

    // Get entity
    Entity* entity = (Entity*)message->obj;
    QStandardItem* item = new QStandardItem(QString::fromStdString(entity->name));
    item->setData(QVariant::fromValue(entity));
    sceneModel->appendRow(item);
}

void MainWindow::slotSceneItemChanged(QStandardItem* item) {
    Entity* entity = item->data().value<Entity*>();
    entity->name = item->text().toStdString();
}

void MainWindow::slotSceneFilter(QString search) {
    QSortFilterProxyModel* model = (QSortFilterProxyModel*)ui->viewScene->model();
    model->setFilterWildcard(search);
}

void MainWindow::on_actionNew_Project_triggered() {
    QString folder = QFileDialog::getExistingDirectory(this, "Select Folder");
}

void MainWindow::sceneItemClicked(QModelIndex idx) {
    QSortFilterProxyModel* proxyModel = (QSortFilterProxyModel*)ui->viewScene->model();
    QStandardItemModel* sceneModel = (QStandardItemModel*)proxyModel->sourceModel();

    QStandardItem* item = sceneModel->item(idx.row(), idx.column());
    if(item) {
        Entity* e = item->data().value<Entity*>();
    }
}
