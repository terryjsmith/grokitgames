/********************************************************************************
** Form generated from reading UI file 'newentitydialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWENTITYDIALOG_H
#define UI_NEWENTITYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewEntityDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewEntityDialog)
    {
        if (NewEntityDialog->objectName().isEmpty())
            NewEntityDialog->setObjectName(QString::fromUtf8("NewEntityDialog"));
        NewEntityDialog->resize(406, 115);
        verticalLayout = new QVBoxLayout(NewEntityDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lineEdit = new QLineEdit(NewEntityDialog);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        buttonBox = new QDialogButtonBox(NewEntityDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewEntityDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewEntityDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewEntityDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewEntityDialog);
    } // setupUi

    void retranslateUi(QDialog *NewEntityDialog)
    {
        NewEntityDialog->setWindowTitle(QCoreApplication::translate("NewEntityDialog", "Add Entity", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("NewEntityDialog", "New entity name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewEntityDialog: public Ui_NewEntityDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWENTITYDIALOG_H
