#ifndef NEWCOMPONENTDIALOG_H
#define NEWCOMPONENTDIALOG_H

#include <QDialog>

namespace Ui {
class NewComponentDialog;
}

class NewComponentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewComponentDialog(QWidget *parent = nullptr);
    ~NewComponentDialog();

    void accept() override;

private:
    Ui::NewComponentDialog *ui;
};

#endif // NEWCOMPONENTDIALOG_H
