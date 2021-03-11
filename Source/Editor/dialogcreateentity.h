#ifndef DIALOGCREATEENTITY_H
#define DIALOGCREATEENTITY_H

#include <QDialog>

namespace Ui {
class DialogCreateEntity;
}

class DialogCreateEntity : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCreateEntity(QWidget *parent = nullptr);
    ~DialogCreateEntity();

    void accept() override;

private:
    Ui::DialogCreateEntity *ui;
};

#endif // DIALOGCREATEENTITY_H
