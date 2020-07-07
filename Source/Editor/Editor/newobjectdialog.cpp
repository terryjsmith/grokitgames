#include "newobjectdialog.h"
#include "ui_newobjectdialog.h"

NewObjectDialog::NewObjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewObjectDialog)
{
    ui->setupUi(this);
}

NewObjectDialog::~NewObjectDialog()
{
    delete ui;
}
