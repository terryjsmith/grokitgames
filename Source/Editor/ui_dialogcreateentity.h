/********************************************************************************
** Form generated from reading UI file 'dialogcreateentity.ui'
**
** Created by: Qt User Interface Compiler version 6.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGCREATEENTITY_H
#define UI_DIALOGCREATEENTITY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DialogCreateEntity
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *txtEntityName;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogCreateEntity)
    {
        if (DialogCreateEntity->objectName().isEmpty())
            DialogCreateEntity->setObjectName(QString::fromUtf8("DialogCreateEntity"));
        DialogCreateEntity->setWindowModality(Qt::ApplicationModal);
        DialogCreateEntity->resize(421, 82);
        DialogCreateEntity->setModal(true);
        verticalLayout = new QVBoxLayout(DialogCreateEntity);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(DialogCreateEntity);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        txtEntityName = new QLineEdit(DialogCreateEntity);
        txtEntityName->setObjectName(QString::fromUtf8("txtEntityName"));

        formLayout->setWidget(0, QFormLayout::FieldRole, txtEntityName);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(DialogCreateEntity);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DialogCreateEntity);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, DialogCreateEntity, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, DialogCreateEntity, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(DialogCreateEntity);
    } // setupUi

    void retranslateUi(QDialog *DialogCreateEntity)
    {
        DialogCreateEntity->setWindowTitle(QCoreApplication::translate("DialogCreateEntity", "Create Entity", nullptr));
        label->setText(QCoreApplication::translate("DialogCreateEntity", "Entity Name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DialogCreateEntity: public Ui_DialogCreateEntity {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGCREATEENTITY_H
