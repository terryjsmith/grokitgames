
#include "propertyformbuilder.h"
#include "mainwindow.h"

#include <IO/ResourceObject.h>

#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QIntValidator>

QFormLayout* PropertyFormBuilder::GetFormLayout(DataRecord* record, QWidget* parent) {
    // Create a new form layout
    QFormLayout* layout = new QFormLayout(0);
    MainWindow* mainWindow = MainWindow::getInstance();

    // Iterate over keys to create layout
    std::vector<std::string> keys = record->GetKeys();
    auto it = keys.begin();
    for(; it != keys.end(); it++) {
        QWidget* widget = 0;
        bool added = false;

        if((*it).compare("entityID") == 0) continue;

        // Set the field name to the column name
        QString fieldName = (*it).c_str();
        fieldName = fieldName.replace(QRegExp("([a-z])([A-Z])"), "\\1 \\2");
        fieldName = fieldName.replace(0, 1, fieldName.left(1).toUpper());

        // Get the variant and type
        Variant* v = record->Get(*it);
        uint32_t variantType = v->GetType();

        if(variantType == Variant::VAR_INT32 || variantType == Variant::VAR_UINT32 || variantType == Variant::VAR_INT64 || variantType == Variant::VAR_UINT64) {
            QLineEdit* edit = new QLineEdit(parent);
            edit->setText(QString::fromStdString(v->ToString()));
            edit->setValidator(new QIntValidator(layout));
            edit->setMaximumWidth(40);
            widget = edit;
        }

        if(variantType == Variant::VAR_BOOL) {
            QCheckBox* cb = new QCheckBox(parent);
            cb->setChecked(v->AsBool());
            connect(cb, SIGNAL(stateChanged(int)), mainWindow, SLOT(cbStateChange(int)));
            widget = cb;
        }

        if(variantType == Variant::VAR_FLOAT) {
            QLineEdit* edit = new QLineEdit(parent);
            edit->setText(QString::fromStdString(v->ToString()));
            edit->setValidator(new QDoubleValidator(layout));
            edit->setMaximumWidth(40);
            widget = edit;
        }

        if(variantType == Variant::VAR_VECTOR2) {
            QHBoxLayout* hbox = new QHBoxLayout();
            vector2 vec2 = v->AsVector2();

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(vec2.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            hbox->addWidget(x);

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(vec2.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            hbox->addWidget(y);

            layout->addRow(fieldName,hbox);
            added = true;
        }

        if(variantType == Variant::VAR_VECTOR3) {
            QHBoxLayout* hbox = new QHBoxLayout();
            vector3 vec3 = v->AsVector3();

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(vec3.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            x->setObjectName(fieldName + " X");
            hbox->addWidget(x);

            x->setProperty("fieldRecord", QVariant::fromValue((void*)record));

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(vec3.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            y->setObjectName(fieldName + " Y");
            hbox->addWidget(y);

            y->setProperty("fieldRecord", QVariant::fromValue((void*)record));

            QLineEdit* z = new QLineEdit(parent);
            z->setText(QString::number(vec3.z, 'f', 2));
            z->setMaximumWidth(40);
            z->setValidator(new QDoubleValidator(layout));
            z->setObjectName(fieldName + " Z");
            hbox->addWidget(z);

            z->setProperty("fieldRecord", QVariant::fromValue((void*)record));

            connect(x, SIGNAL(editingFinished()), mainWindow, SLOT(textEditFinished()));
            connect(y, SIGNAL(editingFinished()), mainWindow, SLOT(textEditFinished()));
            connect(z, SIGNAL(editingFinished()), mainWindow, SLOT(textEditFinished()));

            layout->addRow(fieldName,hbox);
            added = true;
        }

        if(variantType == Variant::VAR_VECTOR4) {
            QHBoxLayout* hbox = new QHBoxLayout();
            vector4 vec4 = v->AsVector4();

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(vec4.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            hbox->addWidget(x);

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(vec4.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            hbox->addWidget(y);

            QLineEdit* z = new QLineEdit(parent);
            z->setText(QString::number(vec4.z, 'f', 2));
            z->setMaximumWidth(40);
            z->setValidator(new QDoubleValidator(layout));
            hbox->addWidget(z);

            QLineEdit* w = new QLineEdit(parent);
            w->setText(QString::number(vec4.w, 'f', 2));
            w->setMaximumWidth(40);
            w->setValidator(new QDoubleValidator(layout));
            hbox->addWidget(w);

            layout->addRow(fieldName,hbox);
            added = true;
        }

        if(variantType == Variant::VAR_QUATERNION) {
            QHBoxLayout* hbox = new QHBoxLayout();
            quaternion quat = v->AsQuaternion();

            vector3 angles = glm::degrees(glm::eulerAngles(quat));

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(angles.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            x->setObjectName(fieldName + " X");
            hbox->addWidget(x);

            x->setProperty("fieldRecord", QVariant::fromValue(record));

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(angles.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            y->setObjectName(fieldName + " Y");
            hbox->addWidget(y);

            y->setProperty("fieldRecord", QVariant::fromValue(record));

            QLineEdit* z = new QLineEdit(parent);
            z->setText(QString::number(angles.z, 'f', 2));
            z->setMaximumWidth(40);
            z->setValidator(new QDoubleValidator(layout));
            z->setObjectName(fieldName + " Z");
            hbox->addWidget(z);

            z->setProperty("fieldRecord", QVariant::fromValue(record));

            connect(x, SIGNAL(editingFinished()), mainWindow, SLOT(textEditFinished()));
            connect(y, SIGNAL(editingFinished()), mainWindow, SLOT(textEditFinished()));
            connect(z, SIGNAL(editingFinished()), mainWindow, SLOT(textEditFinished()));

            layout->addRow(fieldName,hbox);
            added = true;
        }

        if(variantType == Variant::VAR_STRING) {
            QLineEdit* edit = new QLineEdit(parent);
            edit->setText(QString::fromStdString(v->AsString()));
            widget = edit;
        }

        // Special case for resource objects
        if(variantType == Variant::VAR_OBJECT) {
            GigaObject* obj = v->AsObject();
            ResourceObject* resourceObj = dynamic_cast<ResourceObject*>(obj);
            if(resourceObj == 0) {
                QVBoxLayout* vbox = new QVBoxLayout;

                DataRecord* dr2 = new DataRecord();
                obj->Serialize(dr2);

                QFormLayout* form = GetFormLayout(dr2, widget);
                if(form->rowCount() > 1) {
                    QLabel* label = new QLabel(QString::fromStdString(obj->GetGigaName()));
                    label->setStyleSheet("font-weight: bold;");
                    vbox->addWidget(label);
                }
                vbox->addLayout(form);

                layout->addRow(vbox);
                delete dr2;
            }
            else {
                QHBoxLayout* hbox = new QHBoxLayout();

                QLineEdit* edit = new QLineEdit(parent);
                QString filename = QString::fromStdString(resourceObj->GetResource()->filename);
                QString extension = QString::fromStdString(resourceObj->GetResource()->extension);
                edit->setText(filename);
                edit->setObjectName(QString::fromStdString(*it) + "_edit");
                edit->setProperty("type", QString::fromStdString(resourceObj->GetResource()->type));
                edit->setProperty("fieldRecord", QVariant::fromValue(record));
                edit->setDisabled(true);
                connect(edit, SIGNAL(textChanged(QString)), mainWindow, SLOT(textResourceChange(QString)));

                QPushButton* button = new QPushButton("...");
                button->setProperty("extension", extension);
                button->setProperty("type", QString::fromStdString(resourceObj->GetResource()->type));
                button->setObjectName(QString::fromStdString(*it));
                connect(button, SIGNAL(clicked()), mainWindow, SLOT(browseAssets()));

                hbox->addWidget(edit);
                hbox->addWidget(button);

                layout->addRow(QString::fromStdString(resourceObj->GetResource()->GetGigaName()), hbox);
            }
            added = true;
        }

        if(added == false) {
            widget->setProperty("fieldName", QString::fromStdString(*it));
            widget->setProperty("fieldRecord", QVariant::fromValue(record));
            layout->insertRow(0, fieldName, widget);
        }
    }

    return(layout);
}

void PropertyFormBuilder::clearLayout(QLayout *layout) {
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
           delete item->widget();
        }
        //delete item;
    }
}
