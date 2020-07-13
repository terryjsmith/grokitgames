#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QFormLayout>

#include "scenetreeview.h"
#include "scenetreemodel.h"
#include "metatypes.h"
#include "gigaopenglwidget.h"

#include <Core/DataLoader.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Get instance/singleton
    static MainWindow* getInstance();

    // Getters
    SceneTreeModel* GetSceneModel() { return m_sceneTreeModel; }
    SceneTreeView* GetSceneView();
    QVBoxLayout* GetPropertyLayout();
    GigaOpenGLWidget* GetOpenGLWidget();

    // Form generator
    void GetFormLayout(DataRecord* record, GigaObject* obj, QWidget* parent, QGridLayout* layout, QString title);
    void clearLayout(QLayout *layout);

    // Set edit mode in UI
    void SetEditMode(std::string mode);

public slots:
    // Property callbacks
    void cbTextEditFinished();
    void cbStateChange(int s);

    void btnOpenProject_clicked();
    void btnSaveProject_clicked();
    void btnCreateEntity_clicked();
    void btnCreateComponent_clicked();
    void btnCreateScript_clicked();
    void btnGameBuild_clicked();
    void objectBrowser();
    void trySaveFile();
    void btnChangeEditMode();

private:
    Ui::MainWindow *ui;

    // Our view model for the scene
    SceneTreeModel* m_sceneTreeModel;

    // Singleton
    static MainWindow* m_instance;

    // Data loader
    DataLoader* m_dataLoader;
};

#endif // MAINWINDOW_H
