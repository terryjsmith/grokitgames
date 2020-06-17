#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "scenetreemodel.h"

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
    void btnCreateEntity_clicked();

    // Get instance/singleton
    static MainWindow* getInstance();

    // Get scene view
    SceneTreeModel* GetSceneModel() { return m_sceneTreeModel; }

private:
    Ui::MainWindow *ui;

    // Our view model for the scene
    SceneTreeModel* m_sceneTreeModel;

    // Singleton
    static MainWindow* m_instance;
};

#endif // MAINWINDOW_H
