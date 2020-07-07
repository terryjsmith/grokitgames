#ifndef NEWOBJECTDIALOG_H
#define NEWOBJECTDIALOG_H

#include <QDialog>

namespace Ui {
class NewObjectDialog;
}

class NewObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewObjectDialog(QWidget *parent = nullptr);
    ~NewObjectDialog();

private:
    Ui::NewObjectDialog *ui;
};

#endif // NEWOBJECTDIALOG_H
