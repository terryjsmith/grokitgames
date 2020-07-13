/********************************************************************************
** Form generated from reading UI file 'newobjectdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWOBJECTDIALOG_H
#define UI_NEWOBJECTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewObjectDialog
{
public:
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewObjectDialog)
    {
        if (NewObjectDialog->objectName().isEmpty())
            NewObjectDialog->setObjectName(QString::fromUtf8("NewObjectDialog"));
        NewObjectDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(NewObjectDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        buttonBox = new QDialogButtonBox(NewObjectDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewObjectDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewObjectDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewObjectDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewObjectDialog);
    } // setupUi

    void retranslateUi(QDialog *NewObjectDialog)
    {
        NewObjectDialog->setWindowTitle(QCoreApplication::translate("NewObjectDialog", "New Object", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewObjectDialog: public Ui_NewObjectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWOBJECTDIALOG_H
