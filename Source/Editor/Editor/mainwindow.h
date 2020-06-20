#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "scenetreemodel.h"

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

    void btnOpenProject_clicked();
    void btnSaveProject_clicked();
    void btnCreateEntity_clicked();
    void btnCreateComponent_clicked();

    // Get instance/singleton
    static MainWindow* getInstance();

    // Getters
    SceneTreeModel* GetSceneModel() { return m_sceneTreeModel; }
    QVBoxLayout* GetPropertyLayout();

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
