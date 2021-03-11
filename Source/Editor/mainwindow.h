#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>

#include <Core/Message.h>
#include <Core/Entity.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Singleton
    static MainWindow* GetInstance();

    // Get / set selected entity
    Entity* GetCurrentEntity();
    void SetCurrentEntity(Entity* entity);

    // Callback for entity/component changes
    static void UpdateSceneTree(GigaObject* obj, Message* message);

public slots:
    void slotCreateEntityClicked();
    void slotSceneItemChanged(QStandardItem* item);
    void slotSceneFilter(QString search);
    void sceneItemClicked(QModelIndex idx);

private slots:
    void on_actionNew_Project_triggered();

private:
    Ui::MainWindow *ui;

    // Singleton
    static MainWindow* m_instance;
};
#endif // MAINWINDOW_H
