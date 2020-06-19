/********************************************************************************
** Form generated from reading UI file 'newcomponentdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWCOMPONENTDIALOG_H
#define UI_NEWCOMPONENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewComponentDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QComboBox *ddlEntity;
    QLabel *label;
    QLabel *label_2;
    QComboBox *ddlComponentType;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewComponentDialog)
    {
        if (NewComponentDialog->objectName().isEmpty())
            NewComponentDialog->setObjectName(QString::fromUtf8("NewComponentDialog"));
        NewComponentDialog->resize(447, 140);
        verticalLayout = new QVBoxLayout(NewComponentDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        ddlEntity = new QComboBox(NewComponentDialog);
        ddlEntity->setObjectName(QString::fromUtf8("ddlEntity"));

        formLayout->setWidget(0, QFormLayout::FieldRole, ddlEntity);

        label = new QLabel(NewComponentDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        label_2 = new QLabel(NewComponentDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        ddlComponentType = new QComboBox(NewComponentDialog);
        ddlComponentType->setObjectName(QString::fromUtf8("ddlComponentType"));

        formLayout->setWidget(1, QFormLayout::FieldRole, ddlComponentType);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(NewComponentDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewComponentDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewComponentDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewComponentDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewComponentDialog);
    } // setupUi

    void retranslateUi(QDialog *NewComponentDialog)
    {
        NewComponentDialog->setWindowTitle(QCoreApplication::translate("NewComponentDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("NewComponentDialog", "Entity:", nullptr));
        label_2->setText(QCoreApplication::translate("NewComponentDialog", "Component:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewComponentDialog: public Ui_NewComponentDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWCOMPONENTDIALOG_H
