#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QFormLayout>

#include "scenetreemodel.h"
#include "metatypes.h"

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
    QVBoxLayout* GetPropertyLayout();

    // Form generator
    QFormLayout* GetFormLayout(DataRecord* record, GigaObject* obj, QWidget* parent);
    void clearLayout(QLayout *layout);

    // Add override
    void AddOverride(std::string field, std::string type);

public slots:
    // Property callbacks
    void cbTextEditFinished();

    void btnOpenProject_clicked();
    void btnSaveProject_clicked();
    void btnCreateEntity_clicked();
    void btnCreateComponent_clicked();
    void btnCreateScript_clicked();
    void objectBrowser();

private:
    Ui::MainWindow *ui;

    // Our view model for the scene
    SceneTreeModel* m_sceneTreeModel;

    // Singleton
    static MainWindow* m_instance;

    // Data loader
    DataLoader* m_dataLoader;

    // Custom object types
    std::map<std::string, std::string> m_overrides;
};

#endif // MAINWINDOW_H
