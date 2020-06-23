/********************************************************************************
** Form generated from reading UI file 'objectselectdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OBJECTSELECTDIALOG_H
#define UI_OBJECTSELECTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ObjectSelectDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit;
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ObjectSelectDialog)
    {
        if (ObjectSelectDialog->objectName().isEmpty())
            ObjectSelectDialog->setObjectName(QString::fromUtf8("ObjectSelectDialog"));
        ObjectSelectDialog->resize(462, 294);
        verticalLayout = new QVBoxLayout(ObjectSelectDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lineEdit = new QLineEdit(ObjectSelectDialog);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        listWidget = new QListWidget(ObjectSelectDialog);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setAlternatingRowColors(true);

        verticalLayout->addWidget(listWidget);

        buttonBox = new QDialogButtonBox(ObjectSelectDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(ObjectSelectDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ObjectSelectDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ObjectSelectDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ObjectSelectDialog);
    } // setupUi

    void retranslateUi(QDialog *ObjectSelectDialog)
    {
        ObjectSelectDialog->setWindowTitle(QCoreApplication::translate("ObjectSelectDialog", "Object Selector", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("ObjectSelectDialog", "Search...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ObjectSelectDialog: public Ui_ObjectSelectDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OBJECTSELECTDIALOG_H
