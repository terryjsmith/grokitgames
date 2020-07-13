#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newentitydialog.h"
#include "newcomponentdialog.h"
#include "modeltest.h"
#include "objectselectdialog.h"
#include "editorsettings.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QIntValidator>
#include <QShortcut>

#include <Render/RenderSystem.h>
#include <Scripting/ScriptingSystem.h>
#include <IO/SQLiteDataLoader.h>
#include <Core/Application.h>
#include <Core/World.h>
#include <IO/ResourceSystem.h>

MainWindow* MainWindow::m_instance = 0;

MainWindow* MainWindow::getInstance() {
    if(m_instance == 0) {
        m_instance = new MainWindow();
    }

    return(m_instance);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    connect(ui->actionOpen_Project, &QAction::triggered, this, &MainWindow::btnOpenProject_clicked);
    connect(ui->actionSave_Project, &QAction::triggered, this, &MainWindow::btnSaveProject_clicked);
    connect(ui->actionEntity, &QAction::triggered, this, &MainWindow::btnCreateEntity_clicked);
    connect(ui->actionComponent, &QAction::triggered, this, &MainWindow::btnCreateComponent_clicked);
    connect(ui->actionScript, &QAction::triggered, this, &MainWindow::btnCreateScript_clicked);
    connect(ui->btnEditMode_Grab, &QPushButton::clicked, this, &MainWindow::btnChangeEditMode);
    connect(ui->btnEditMode_Move, &QPushButton::clicked, this, &MainWindow::btnChangeEditMode);
    connect(ui->btnEditMode_Rotate, &QPushButton::clicked, this, &MainWindow::btnChangeEditMode);
    connect(ui->btnEditMode_Scale, &QPushButton::clicked, this, &MainWindow::btnChangeEditMode);

    // Set up edit mode
    ui->btnEditMode_Grab->setProperty("editMode", (int)GigaOpenGLWidget::EditMode::EDITMODE_GRAB);
    ui->btnEditMode_Move->setProperty("editMode", (int)GigaOpenGLWidget::EditMode::EDITMODE_MOVE);
    ui->btnEditMode_Rotate->setProperty("editMode", (int)GigaOpenGLWidget::EditMode::EDITMODE_ROTATE);
    ui->btnEditMode_Scale->setProperty("editMode", (int)GigaOpenGLWidget::EditMode::EDITMODE_SCALE);

    // Set default edit mode
    ui->btnEditMode_Grab->setChecked(true);

    ui->btnNewEntity->setDefaultAction(ui->actionEntity);
    ui->btnNewEntity->setText("+");

    ui->tabWidget_3->removeTab(1);

    // Create short key for saving files
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(trySaveFile()));

    // Initialize tree model
    m_sceneTreeModel = new SceneTreeModel(0);
    ui->sceneView->setModel(m_sceneTreeModel);

    ModelTest test(m_sceneTreeModel);

    this->installEventFilter(ui->openGLWidget);
    ui->sceneView->initialized = true;

    ui->openGLWidget->setFocus(Qt::FocusReason::NoFocusReason);
}

void MainWindow::btnOpenProject_clicked() {
    // Have the user choose a project directory
    QString filePath = QFileDialog::getOpenFileName(this, "Open Project", QDir::currentPath(), "Eternity Project DC (*.gpf)");
    if(filePath.length() == 0) {
        return;
    }

    QString path = filePath;
    QString directory = path = path.mid(0, path.lastIndexOf("/"));

    QFile projFile(filePath);
    if(!projFile.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open project settings file.");
        return;
    }

    QByteArray jsonData = projFile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
    QJsonObject settingsObject = jsonDoc.object();

    setWindowTitle("GIGA Editor - " + settingsObject["name"].toString());

    //m_currentProjectDirectory = directory;
    QDir::setCurrent(directory);

    // Add search paths
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    resourceSystem->AddSearchPath("Resources/Textures");
    resourceSystem->AddSearchPath("Resources/Models");
    resourceSystem->AddSearchPath("Resources/Shaders");
    resourceSystem->AddSearchPath("Resources/Audio");

    // Load a game.dll file if one exists
    QString gameDllFile = directory + "/game.dll";
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();

    if(QFile::exists(gameDllFile)) {
        scriptingSystem->LoadLibrary(gameDllFile.toStdString());
    }

    m_dataLoader = new SQLiteDataLoader();
    Application::GetInstance()->RegisterDataLoader("game", m_dataLoader);

    // Look for a game.db file
    QString gamedbFile = directory + "/game.db";
    m_dataLoader->Open(gamedbFile.toStdString());
    if(QFile::exists(gamedbFile)) {
        World* world = World::GetInstance();
        MetaSystem* metaSystem = GetSystem<MetaSystem>();

        // Load entities
        Array<GigaObject*> entities = m_dataLoader->GetObjects("Entity");

        QStandardItem* rootItem = m_sceneTreeModel->invisibleRootItem();

        // Add entities to world
        auto it = entities.begin();
        for(; it != entities.end(); it++) {
            Entity* entity = dynamic_cast<Entity*>(*it);
            world->AddEntity(entity);
            QStandardItem* item = new QStandardItem(entity->name.c_str());
            QVariant value = qVariantFromValue((void*)entity);
            item->setData(value);
            rootItem->appendRow(item);
        }

        // Load components
        Array<std::string> componentTypes = metaSystem->GetComponentTypes();
        auto cti = componentTypes.begin();
        for(; cti != componentTypes.end(); cti++) {
            Array<GigaObject*> components = m_dataLoader->GetObjects(*cti);
            auto ci = components.begin();
            for(; ci != components.end(); ci++) {
                // Check whether this is attached to an entity
                Component* component = dynamic_cast<Component*>(*ci);
                Entity* entity = component->GetParent();
                if(entity != 0) {
                    QStandardItem* item = m_sceneTreeModel->findItem(entity->entityID);

                    QStandardItem* newItem = new QStandardItem((*cti).c_str());
                    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    item->appendRow(newItem);
                }
            }
        }
    }

    // Enable menu items
    ui->actionClose_Project->setEnabled(true);
    ui->actionNew_Scene->setEnabled(true);
    ui->actionOpen_Scene->setEnabled(true);
    ui->actionSave_Project->setEnabled(true);
}

void MainWindow::btnSaveProject_clicked() {
    World* world = World::GetInstance();
    MetaSystem* metaSystem = GetSystem<MetaSystem>();

    // Save entities
    Array<Entity*> entities = world->GetEntities();

    Array<GigaObject*> objs;
    auto it = entities.begin();
    for(; it != entities.end(); it++) {
        objs.push_back(*it);
    }

    m_dataLoader->SaveObjects("Entity", objs);
    objs.clear();

    // Save components
    Array<std::string> componentTypes = metaSystem->GetComponentTypes();
    auto cti = componentTypes.begin();
    for(; cti != componentTypes.end(); cti++) {
        Array<Component*> components = world->FindComponents(*cti);

        auto it = components.begin();
        for(; it != components.end(); it++) {
            objs.push_back(*it);
        }

        m_dataLoader->SaveObjects(*cti, objs);

        objs.clear();
    }
}

void MainWindow::btnCreateEntity_clicked() {
    NewEntityDialog* dlg = new NewEntityDialog(this);
    dlg->setModal(true);
    dlg->show();
}

void MainWindow::btnCreateComponent_clicked() {
    NewComponentDialog* dlg = new NewComponentDialog(this);
    dlg->setModal(true);
    dlg->show();
}

void MainWindow::btnCreateScript_clicked() {
    QPlainTextEdit* editWidget = new QPlainTextEdit(this);
    int index = ui->tabWidget_3->addTab(editWidget, "Untitled *");
    ui->tabWidget_3->setCurrentIndex(index);

    // Populate with default script
    QString contents = "using System;\nusing GIGA;\n\nnamespace Game {\n\n}\n";
    editWidget->setPlainText(contents);
}

QVBoxLayout* MainWindow::GetPropertyLayout() {
    return(ui->propertiesLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cbTextEditFinished() {
    // Get our signal sender
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());

    // Get our object
    GigaObject* obj = edit->property("fieldObject").value<GigaObject*>();

    // Serialize
    DataRecord* dr = new DataRecord();
    obj->Serialize(dr);

    // Get which field this is
    QString qFieldName = edit->property("fieldName").value<QString>();
    std::string fieldName = qFieldName.toStdString();
    std::string fieldValue = edit->text().toStdString();

    // Get the variant we're updating
    Variant* v = dr->Get(fieldName);
    switch(v->GetType()) {
    case Variant::VAR_FLOAT:
        dr->Set(fieldName, new Variant((float)atof(fieldValue.c_str())));
        break;
    case Variant::VAR_INT32:
        dr->Set(fieldName, new Variant((int32_t)atoi(fieldValue.c_str())));
        break;
    case Variant::VAR_UINT32:
        dr->Set(fieldName, new Variant((uint32_t)atoi(fieldValue.c_str())));
        break;
    case Variant::VAR_INT64:
        dr->Set(fieldName, new Variant((int64_t)atoi(fieldValue.c_str())));
        break;
    case Variant::VAR_UINT64:
        dr->Set(fieldName, new Variant((uint64_t)atoi(fieldValue.c_str())));
        break;
    case Variant::VAR_STRING:
        dr->Set(fieldName, new Variant(fieldValue));
        break;
    default: break;
    }

    QLineEdit* editX = 0;
    QLineEdit* editY = 0;
    QLineEdit* editZ = 0;
    QLineEdit* editW = 0;

    QList<QLineEdit*> children = ui->propertiesFrame->findChildren<QLineEdit*>(qFieldName + "_X");
    for(int i = 0; i < children.size(); i++) {
        if(children[i]->property("fieldObject").value<GigaObject*>() == obj) {
            editX = children[i];
        }
    }

    children = ui->propertiesFrame->findChildren<QLineEdit*>(qFieldName + "_Y");
    for(int i = 0; i < children.size(); i++) {
        if(children[i]->property("fieldObject").value<GigaObject*>() == obj) {
            editY = children[i];
        }
    }

    children = ui->propertiesFrame->findChildren<QLineEdit*>(qFieldName + "_Z");
    for(int i = 0; i < children.size(); i++) {
        if(children[i]->property("fieldObject").value<GigaObject*>() == obj) {
            editZ = children[i];
        }
    }

    children = ui->propertiesFrame->findChildren<QLineEdit*>(qFieldName + "_W");
    for(int i = 0; i < children.size(); i++) {
        if(children[i]->property("fieldObject").value<GigaObject*>() == obj) {
            editW = children[i];
        }
    }

    // Special handling for objects
    if(v->GetType() == Variant::VAR_OBJECT) {
        // Get results
        QList<GigaObject*> objects = edit->property("results").value<QList<GigaObject*>>();
        if(objects.size() == 1) {
            dr->Set(fieldName, new Variant(objects[0]));
        }
        else {
            assert(false); // Need list handling
        }
    }

    // Special handling for vectors / quaternions
    if(v->GetType() == Variant::VAR_VECTOR2) {
        std::string X = editX->hasAcceptableInput() ? editX->text().toStdString() : std::to_string(v->AsVector2().x);
        std::string Y = editY->hasAcceptableInput() ? editY->text().toStdString() : std::to_string(v->AsVector2().y);

        vector2 newVec = vector2((float)atof(X.c_str()), (float)atof(Y.c_str()));

        dr->Set(fieldName, new Variant(newVec));
    }

    if(v->GetType() == Variant::VAR_VECTOR3) {
        std::string X = editX->hasAcceptableInput() ? editX->text().toStdString() : std::to_string(v->AsVector3().x);
        std::string Y = editY->hasAcceptableInput() ? editY->text().toStdString() : std::to_string(v->AsVector3().y);
        std::string Z = editZ->hasAcceptableInput() ? editZ->text().toStdString() : std::to_string(v->AsVector3().z);

        vector3 newVec = vector3((float)atof(X.c_str()), (float)atof(Y.c_str()), (float)atof(Z.c_str()));

        dr->Set(fieldName, new Variant(newVec));
    }

    if(v->GetType() == Variant::VAR_VECTOR4) {
        vector4 newVec = vector4((float)atof(editX->text().toStdString().c_str()),
                                 (float)atof(editY->text().toStdString().c_str()),
                                 (float)atof(editZ->text().toStdString().c_str()),
                                 (float)atof(editW->text().toStdString().c_str()));

        dr->Set(fieldName, new Variant(newVec));
    }

    if(v->GetType() == Variant::VAR_QUATERNION) {
        vector3 newVec = vector3(glm::radians((float)atof(editX->text().toStdString().c_str())),
                                 glm::radians((float)atof(editY->text().toStdString().c_str())),
                                 glm::radians((float)atof(editZ->text().toStdString().c_str())));
        quaternion q = quaternion(newVec);
        dr->Set(fieldName, new Variant(q));
    }

    // Set values into obj
    obj->Deserialize(dr);

    // Clean up
    delete dr;
}


void MainWindow::GetFormLayout(DataRecord* record, GigaObject* obj, QWidget* parent, QGridLayout* layout, QString title) {
    // Iterate over keys to create layout
    Array<std::string> keys = record->GetKeys();

    QLabel* label = new QLabel(title);
    label->setStyleSheet("font-weight: bold; background: #aaa; padding: 5px;");

    layout->addWidget(label, layout->rowCount() + 1, 0, 1, 2);

    // Start by grouping everything together by part (keys are stored as part.varname)
    std::map<std::string, Variant*> mapped;
    auto it = keys.begin();
    for(; it != keys.end(); it++) {
        mapped[*it] = record->Get(*it);
    }

    std::string currentBlock = "";
    it = keys.begin();
    for(; it != keys.end(); it++) {
        QWidget* widget = 0;
        bool added = false;

        if((*it).compare("Component.entityID") == 0) continue;

        // See if we're starting a new block
        std::string block = (*it).substr(0, (*it).find("."));
        if(currentBlock != block) {
            // If we're switching blocks, show a header of sorts
            QLabel* label = new QLabel(QString::fromStdString(block));
            label->setStyleSheet("font-weight: bold;");

            layout->addWidget(label, layout->rowCount() + 1, 0, 1, 2);
            //layout->addRow(QString::fromStdString(block), spacer);

            currentBlock = block;
        }

        // Set the field name to the column name
        QString origFieldName = (*it).c_str();
        QString fieldName = (*it).c_str();
        fieldName = fieldName.mid(fieldName.indexOf(".") + 1);
        fieldName = fieldName.replace(QRegExp("([a-z])([A-Z])"), "\\1 \\2");
        fieldName = fieldName.replace(0, 1, fieldName.left(1).toUpper());

        // Add the label
        QLabel* label = new QLabel(fieldName);
        layout->addWidget(label, layout->rowCount(), 0);

        // Get the variant and type
        Variant* v = record->Get(*it);
        uint32_t variantType = v->GetType();

        // Get any custom field definition
        EditorSettings::CustomField* customField = EditorSettings::GetInstance()->GetCustomField(obj->GetGigaName(), *it);

        if(variantType == Variant::VAR_INT32 || variantType == Variant::VAR_UINT32 || variantType == Variant::VAR_INT64 || variantType == Variant::VAR_UINT64) {
            std::string value = v->ToString();
            if(customField && value.length() == 0) value = customField->defaultValue;

            QLineEdit* edit = new QLineEdit(parent);
            edit->setText(QString::fromStdString(value));
            edit->setValidator(new QIntValidator(layout));
            edit->setMaximumWidth(40);
            widget = edit;

            connect(edit, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
        }

        if(variantType == Variant::VAR_BOOL) {
            bool value = v->AsBool();

            QCheckBox* cb = new QCheckBox(parent);
            cb->setChecked(value);

            connect(cb, SIGNAL(stateChanged(int)), this, SLOT(cbStateChange(int)));
            widget = cb;
        }

        if(variantType == Variant::VAR_FLOAT) {
            float value = v->AsFloat();

            QLineEdit* edit = new QLineEdit(parent);
            edit->setText(QString::number(value, 'f', 2));
            edit->setValidator(new QDoubleValidator(layout));
            edit->setMaximumWidth(40);
            widget = edit;

            connect(edit, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
        }

        if(variantType == Variant::VAR_VECTOR2) {
            vector2 value = v->AsVector2();
            QHBoxLayout* hbox = new QHBoxLayout();
            vector2 vec2 = value;

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(vec2.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            x->setObjectName(origFieldName + "_X");
            hbox->addWidget(x);

            x->setProperty("fieldObject", QVariant::fromValue(obj));
            x->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(vec2.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            y->setObjectName(origFieldName + "_Y");
            hbox->addWidget(y);

            y->setProperty("fieldObject", QVariant::fromValue(obj));
            y->setProperty("fieldName", QString::fromStdString(*it));

            connect(x, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(y, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));

            //hbox->addStretch(1);
            layout->addLayout(hbox, layout->rowCount() - 1, 1);
            added = true;
        }

        if(variantType == Variant::VAR_VECTOR3) {
            vector3 value = v->AsVector3();
            QHBoxLayout* hbox = new QHBoxLayout();
            vector3 vec3 = value;

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(vec3.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            x->setObjectName(origFieldName + "_X");
            hbox->addWidget(x);

            x->setProperty("fieldObject", QVariant::fromValue(obj));
            x->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(vec3.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            y->setObjectName(origFieldName + "_Y");
            hbox->addWidget(y);

            y->setProperty("fieldObject", QVariant::fromValue(obj));
            y->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* z = new QLineEdit(parent);
            z->setText(QString::number(vec3.z, 'f', 2));
            z->setMaximumWidth(40);
            z->setValidator(new QDoubleValidator(layout));
            z->setObjectName(origFieldName + "_Z");
            hbox->addWidget(z);

            z->setProperty("fieldObject", QVariant::fromValue(obj));
            z->setProperty("fieldName", QString::fromStdString(*it));

            connect(x, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(y, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(z, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));

            //hbox->addStretch(1);
            layout->addLayout(hbox, layout->rowCount() - 1, 1);
            added = true;
        }

        if(variantType == Variant::VAR_VECTOR4) {
            vector4 value = v->AsVector4();
            QHBoxLayout* hbox = new QHBoxLayout();
            vector4 vec4 = value;

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(vec4.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            x->setObjectName(origFieldName + "_X");
            hbox->addWidget(x);

            x->setProperty("fieldObject", QVariant::fromValue(obj));
            x->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(vec4.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            y->setObjectName(origFieldName + "_Y");
            hbox->addWidget(y);

            y->setProperty("fieldObject", QVariant::fromValue(obj));
            y->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* z = new QLineEdit(parent);
            z->setText(QString::number(vec4.z, 'f', 2));
            z->setMaximumWidth(40);
            z->setValidator(new QDoubleValidator(layout));
            z->setObjectName(origFieldName + "_Z");
            hbox->addWidget(z);

            z->setProperty("fieldObject", QVariant::fromValue(obj));
            z->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* w = new QLineEdit(parent);
            w->setText(QString::number(vec4.w, 'f', 2));
            w->setMaximumWidth(40);
            w->setValidator(new QDoubleValidator(layout));
            w->setObjectName(origFieldName + "_W");
            hbox->addWidget(w);

            w->setProperty("fieldObject", QVariant::fromValue(obj));
            w->setProperty("fieldName", QString::fromStdString(*it));

            connect(x, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(y, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(z, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(w, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));

            //hbox->addStretch(1);
            layout->addLayout(hbox, layout->rowCount() - 1, 1);
            added = true;
        }

        if(variantType == Variant::VAR_QUATERNION) {
            quaternion value = v->AsQuaternion();
            QHBoxLayout* hbox = new QHBoxLayout();
            quaternion quat = value;

            vector3 angles = glm::degrees(glm::eulerAngles(quat));

            QLineEdit* x = new QLineEdit(parent);
            x->setText(QString::number(angles.x, 'f', 2));
            x->setMaximumWidth(40);
            x->setValidator(new QDoubleValidator(layout));
            x->setObjectName(origFieldName + "_X");
            hbox->addWidget(x);

            x->setProperty("fieldObject", QVariant::fromValue(obj));
            x->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* y = new QLineEdit(parent);
            y->setText(QString::number(angles.y, 'f', 2));
            y->setMaximumWidth(40);
            y->setValidator(new QDoubleValidator(layout));
            y->setObjectName(origFieldName + "_Y");
            hbox->addWidget(y);

            y->setProperty("fieldObject", QVariant::fromValue(obj));
            y->setProperty("fieldName", QString::fromStdString(*it));

            QLineEdit* z = new QLineEdit(parent);
            z->setText(QString::number(angles.z, 'f', 2));
            z->setMaximumWidth(40);
            z->setValidator(new QDoubleValidator(layout));
            z->setObjectName(origFieldName + "_Z");
            hbox->addWidget(z);

            z->setProperty("fieldObject", QVariant::fromValue(obj));
            z->setProperty("fieldName", QString::fromStdString(*it));

            connect(x, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(y, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));
            connect(z, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));

            //hbox->addStretch(1);
            layout->addLayout(hbox, layout->rowCount() - 1, 1);
            added = true;
        }

        if(variantType == Variant::VAR_STRING) {
            QHBoxLayout* hbox = new QHBoxLayout();

            std::string value = v->AsString();
            if(customField && value.length() == 0) value = customField->defaultValue;

            QLineEdit* edit = new QLineEdit(parent);
            edit->setDisabled(true);
            edit->setObjectName(QString::fromStdString(*it) + "_edit");
            edit->setProperty("fieldObject", QVariant::fromValue(obj));
            edit->setProperty("fieldName", QString::fromStdString(*it));
            edit->setText(QString::fromStdString(value));
            hbox->addWidget(edit);

            connect(edit, SIGNAL(editingFinished()), this, SLOT(cbTextEditFinished()));

            if(customField) {
                QPushButton* button = new QPushButton();
                button->setText("+");
                button->setProperty("type", QString::fromStdString(customField->type));
                button->setProperty("multiselect", customField->action == "SelectMultiple");
                button->setObjectName(QString::fromStdString(*it));
                connect(button, SIGNAL(clicked()), this, SLOT(objectBrowser()));
                hbox->addWidget(button);
            }

            layout->addLayout(hbox, layout->rowCount() - 1, 1);
            added = true;
        }

        if(added == false) {
            widget->setProperty("fieldObject", QVariant::fromValue(obj));
            widget->setProperty("fieldName", QString::fromStdString(*it));
            layout->addWidget(widget, layout->rowCount() - 1, 1);
        }
    }
}

void MainWindow::cbStateChange(int s) {
    // Get our signal sender
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());

    // Get our object
    GigaObject* obj = checkbox->property("fieldObject").value<GigaObject*>();

    // Serialize
    DataRecord* dr = new DataRecord();
    obj->Serialize(dr);

    // Get which field this is
    QString qFieldName = checkbox->property("fieldName").value<QString>();
    std::string fieldName = qFieldName.toStdString();

    bool isChecked = checkbox->isChecked();
    dr->Set(fieldName, new Variant(isChecked));

    obj->Deserialize(dr);

    delete dr;
}

SceneTreeView* MainWindow::GetSceneView() {
    return(ui->sceneView);
}

void MainWindow::clearLayout(QLayout *layout) {
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
            continue;
        }
        if (item->widget()) {
            delete item->widget();
            continue;
        }
        //delete item;
    }
}

void MainWindow::objectBrowser() {
    // Get sender
    QPushButton* button = (QPushButton*)sender();

    // Get type filter and multi-selection
    QString type = button->property("type").value<QString>();
    bool multiselect = button->property("multiselect").value<bool>();

    ObjectSelectDialog* dlg = new ObjectSelectDialog(type.toStdString(), this);
    dlg->SetSelectMode(multiselect);
    dlg->setModal(true);
    int returnCode = dlg->exec();

    if(returnCode == QDialog::Accepted) {
        QList<GigaObject*> objects = dlg->GetSelectedObjects();

        if(objects.size() == 0) return;

        // Get the object that sent this, update property and send back
        QString name = button->objectName();
        QLineEdit* edit = button->parent()->findChild<QLineEdit*>(name + "_edit");
        edit->setProperty("results", QVariant::fromValue(objects));

        if(objects.size() == 1) {
            edit->setText(QString::fromStdString(objects[0]->ToString()));
        }
        else {
            edit->setText("[multiple]");
        }
    }
}

void MainWindow::btnGameBuild_clicked() {

}

void MainWindow::trySaveFile() {
    int currentIndex = ui->tabWidget_3->currentIndex();
    if(currentIndex == 0) {
        return;
    }

    QPlainTextEdit* edit = (QPlainTextEdit*)ui->tabWidget_3->widget(currentIndex);

    // Determine which file is open in the current tab
    QString filename = ui->tabWidget_3->tabText(currentIndex);
    if(filename.startsWith("Untitled")) {
        filename = QFileDialog::getSaveFileName(this);
        if(filename.isEmpty()) return;

        QString tabText = filename.mid(filename.lastIndexOf("/") + 1);
        ui->tabWidget_3->setTabText(currentIndex, tabText);
        edit->setProperty("fullPath", filename);
    }
    else {
        filename = edit->property("fullPath").value<QString>();
    }

    // Save contents
    QString contents = edit->toPlainText();

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        assert(false);
    }

    file.write(contents.toUtf8());

    file.close();
}

GigaOpenGLWidget* MainWindow::GetOpenGLWidget() {
    return(ui->openGLWidget);
}

void MainWindow::SetEditMode(std::string mode) {
    QPushButton* btn = ui->frame_2->findChild<QPushButton*>(QString::fromStdString("btnEditMode_" + mode));
    btn->click();
}

void MainWindow::btnChangeEditMode() {
    QPushButton* button = (QPushButton*)sender();

    // Get and disable the other button modes
    QFrame* frame = (QFrame*)button->parentWidget();
    QList<QPushButton*> children = frame->findChildren<QPushButton*>();
    for(int i = 0; i < children.size(); i++) {
        if(children[i] != button) {
            children[i]->setChecked(false);
        }
    }

    GigaOpenGLWidget::EditMode mode = (GigaOpenGLWidget::EditMode)button->property("editMode").value<int>();
    ui->openGLWidget->SetEditMode(mode);
}
