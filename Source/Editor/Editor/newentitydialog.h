#ifndef NEWENTITYDIALOG_H
#define NEWENTITYDIALOG_H

#include <QDialog>

namespace Ui {
class NewEntityDialog;
}

class NewEntityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEntityDialog(QWidget *parent = nullptr);
    ~NewEntityDialog();

    void accept() override;

private:
    Ui::NewEntityDialog *ui;
};

#endif // NEWENTITYDIALOG_H
