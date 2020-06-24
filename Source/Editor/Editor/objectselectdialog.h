#ifndef OBJECTSELECTDIALOG_H
#define OBJECTSELECTDIALOG_H

#include <QDialog>

#include <Core/GigaObject.h>

namespace Ui {
class ObjectSelectDialog;
}

class ObjectSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectSelectDialog(std::string filter, QWidget *parent = nullptr);
    ~ObjectSelectDialog();

    void SetSelectMode(bool multiselect);
    QList<GigaObject*> GetSelectedObjects();

private:
    Ui::ObjectSelectDialog *ui;
};

#endif // OBJECTSELECTDIALOG_H
