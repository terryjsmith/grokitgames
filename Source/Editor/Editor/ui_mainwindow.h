/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gigaopenglwidget.h"
#include "scenetreeview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Project;
    QAction *actionOpen_Project;
    QAction *actionClose_Project;
    QAction *actionExit;
    QAction *actionNew_Scene;
    QAction *actionOpen_Scene;
    QAction *actionClose_Scene;
    QAction *actionEntity;
    QAction *actionComponent;
    QAction *actionSave_Project;
    QAction *actionScript;
    QAction *actionBuild;
    QAction *actionRun;
    QAction *actionDeploy;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *toolsLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_4;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *btnEditMode_Grab;
    QPushButton *btnEditMode_Move;
    QPushButton *btnEditMode_Rotate;
    QPushButton *btnEditMode_Scale;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_6;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *btnEditMode_TerrainAdd;
    QPushButton *btnEditMode_TerrainRaise;
    QPushButton *btnEditMode_TerrainFlatten;
    QPushButton *btnEditMode_TerrainPaint;
    QSpacerItem *horizontalSpacer;
    QTabWidget *tabWidget_2;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *sceneSearch;
    QToolButton *btnNewEntity;
    SceneTreeView *sceneView;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_3;
    QLineEdit *resourceSearch;
    QListView *resourceView;
    QWidget *tab_7;
    QVBoxLayout *verticalLayout_6;
    QGridLayout *gridLayout;
    QVBoxLayout *openGLlayout;
    QTabWidget *tabWidget_3;
    QWidget *tab_5;
    QVBoxLayout *verticalLayout_2;
    GigaOpenGLWidget *openGLWidget;
    QWidget *tab_6;
    QVBoxLayout *verticalLayout_5;
    QPlainTextEdit *plainTextEdit;
    QVBoxLayout *verticalLayout_4;
    QFrame *propertiesFrame;
    QVBoxLayout *verticalLayout_7;
    QVBoxLayout *propertiesLayout;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuAdd;
    QMenu *menuGame;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(711, 375);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        actionNew_Project = new QAction(MainWindow);
        actionNew_Project->setObjectName(QString::fromUtf8("actionNew_Project"));
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        actionClose_Project = new QAction(MainWindow);
        actionClose_Project->setObjectName(QString::fromUtf8("actionClose_Project"));
        actionClose_Project->setEnabled(false);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionNew_Scene = new QAction(MainWindow);
        actionNew_Scene->setObjectName(QString::fromUtf8("actionNew_Scene"));
        actionNew_Scene->setEnabled(false);
        actionOpen_Scene = new QAction(MainWindow);
        actionOpen_Scene->setObjectName(QString::fromUtf8("actionOpen_Scene"));
        actionOpen_Scene->setEnabled(false);
        actionClose_Scene = new QAction(MainWindow);
        actionClose_Scene->setObjectName(QString::fromUtf8("actionClose_Scene"));
        actionClose_Scene->setEnabled(false);
        actionEntity = new QAction(MainWindow);
        actionEntity->setObjectName(QString::fromUtf8("actionEntity"));
        actionComponent = new QAction(MainWindow);
        actionComponent->setObjectName(QString::fromUtf8("actionComponent"));
        actionSave_Project = new QAction(MainWindow);
        actionSave_Project->setObjectName(QString::fromUtf8("actionSave_Project"));
        actionSave_Project->setEnabled(false);
        actionScript = new QAction(MainWindow);
        actionScript->setObjectName(QString::fromUtf8("actionScript"));
        actionBuild = new QAction(MainWindow);
        actionBuild->setObjectName(QString::fromUtf8("actionBuild"));
        actionRun = new QAction(MainWindow);
        actionRun->setObjectName(QString::fromUtf8("actionRun"));
        actionDeploy = new QAction(MainWindow);
        actionDeploy->setObjectName(QString::fromUtf8("actionDeploy"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        toolsLayout = new QVBoxLayout();
        toolsLayout->setSpacing(-1);
        toolsLayout->setObjectName(QString::fromUtf8("toolsLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy2);
        tabWidget->setMinimumSize(QSize(200, 0));
        tabWidget->setAutoFillBackground(false);
        tabWidget->setDocumentMode(true);
        tabWidget->setTabBarAutoHide(false);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        horizontalLayout_4 = new QHBoxLayout(tab);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        frame_2 = new QFrame(tab);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMaximumSize(QSize(16777215, 200));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame_2);
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        btnEditMode_Grab = new QPushButton(frame_2);
        btnEditMode_Grab->setObjectName(QString::fromUtf8("btnEditMode_Grab"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/grab.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_Grab->setIcon(icon);
        btnEditMode_Grab->setCheckable(true);
        btnEditMode_Grab->setAutoDefault(false);
        btnEditMode_Grab->setFlat(false);

        horizontalLayout_5->addWidget(btnEditMode_Grab);

        btnEditMode_Move = new QPushButton(frame_2);
        btnEditMode_Move->setObjectName(QString::fromUtf8("btnEditMode_Move"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Icons/drag.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_Move->setIcon(icon1);
        btnEditMode_Move->setCheckable(true);

        horizontalLayout_5->addWidget(btnEditMode_Move);

        btnEditMode_Rotate = new QPushButton(frame_2);
        btnEditMode_Rotate->setObjectName(QString::fromUtf8("btnEditMode_Rotate"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Icons/rotate.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_Rotate->setIcon(icon2);
        btnEditMode_Rotate->setCheckable(true);

        horizontalLayout_5->addWidget(btnEditMode_Rotate);

        btnEditMode_Scale = new QPushButton(frame_2);
        btnEditMode_Scale->setObjectName(QString::fromUtf8("btnEditMode_Scale"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Icons/scale.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_Scale->setIcon(icon3);
        btnEditMode_Scale->setCheckable(true);

        horizontalLayout_5->addWidget(btnEditMode_Scale);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);


        horizontalLayout_4->addWidget(frame_2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        horizontalLayout_6 = new QHBoxLayout(tab_2);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(tab_2);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_7 = new QHBoxLayout(frame);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        btnEditMode_TerrainAdd = new QPushButton(frame);
        btnEditMode_TerrainAdd->setObjectName(QString::fromUtf8("btnEditMode_TerrainAdd"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Icons/plus.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_TerrainAdd->setIcon(icon4);
        btnEditMode_TerrainAdd->setCheckable(false);

        horizontalLayout_7->addWidget(btnEditMode_TerrainAdd);

        btnEditMode_TerrainRaise = new QPushButton(frame);
        btnEditMode_TerrainRaise->setObjectName(QString::fromUtf8("btnEditMode_TerrainRaise"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Icons/terrain_hills.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_TerrainRaise->setIcon(icon5);
        btnEditMode_TerrainRaise->setCheckable(true);
        btnEditMode_TerrainRaise->setFlat(false);

        horizontalLayout_7->addWidget(btnEditMode_TerrainRaise);

        btnEditMode_TerrainFlatten = new QPushButton(frame);
        btnEditMode_TerrainFlatten->setObjectName(QString::fromUtf8("btnEditMode_TerrainFlatten"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Icons/terrain_flatten.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_TerrainFlatten->setIcon(icon6);
        btnEditMode_TerrainFlatten->setCheckable(true);

        horizontalLayout_7->addWidget(btnEditMode_TerrainFlatten);

        btnEditMode_TerrainPaint = new QPushButton(frame);
        btnEditMode_TerrainPaint->setObjectName(QString::fromUtf8("btnEditMode_TerrainPaint"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Icons/terrain_paint.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditMode_TerrainPaint->setIcon(icon7);
        btnEditMode_TerrainPaint->setCheckable(true);

        horizontalLayout_7->addWidget(btnEditMode_TerrainPaint);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);


        horizontalLayout_6->addWidget(frame);

        tabWidget->addTab(tab_2, QString());

        toolsLayout->addWidget(tabWidget);

        tabWidget_2 = new QTabWidget(centralWidget);
        tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
        tabWidget_2->setStyleSheet(QString::fromUtf8(""));
        tabWidget_2->setTabShape(QTabWidget::Rounded);
        tabWidget_2->setElideMode(Qt::ElideRight);
        tabWidget_2->setDocumentMode(true);
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout = new QVBoxLayout(tab_3);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, -1, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, -1, -1, -1);
        sceneSearch = new QLineEdit(tab_3);
        sceneSearch->setObjectName(QString::fromUtf8("sceneSearch"));

        horizontalLayout_3->addWidget(sceneSearch);

        btnNewEntity = new QToolButton(tab_3);
        btnNewEntity->setObjectName(QString::fromUtf8("btnNewEntity"));

        horizontalLayout_3->addWidget(btnNewEntity);


        verticalLayout->addLayout(horizontalLayout_3);

        sceneView = new SceneTreeView(tab_3);
        sceneView->setObjectName(QString::fromUtf8("sceneView"));
        sceneView->setFrameShape(QFrame::StyledPanel);
        sceneView->setAlternatingRowColors(true);
        sceneView->setHeaderHidden(true);
        sceneView->header()->setVisible(false);

        verticalLayout->addWidget(sceneView);

        tabWidget_2->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        verticalLayout_3 = new QVBoxLayout(tab_4);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, -1, 0, 0);
        resourceSearch = new QLineEdit(tab_4);
        resourceSearch->setObjectName(QString::fromUtf8("resourceSearch"));

        verticalLayout_3->addWidget(resourceSearch);

        resourceView = new QListView(tab_4);
        resourceView->setObjectName(QString::fromUtf8("resourceView"));

        verticalLayout_3->addWidget(resourceView);

        tabWidget_2->addTab(tab_4, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QString::fromUtf8("tab_7"));
        verticalLayout_6 = new QVBoxLayout(tab_7);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        verticalLayout_6->addLayout(gridLayout);

        tabWidget_2->addTab(tab_7, QString());

        toolsLayout->addWidget(tabWidget_2);

        toolsLayout->setStretch(1, 1);

        horizontalLayout->addLayout(toolsLayout);

        openGLlayout = new QVBoxLayout();
        openGLlayout->setSpacing(6);
        openGLlayout->setObjectName(QString::fromUtf8("openGLlayout"));
        tabWidget_3 = new QTabWidget(centralWidget);
        tabWidget_3->setObjectName(QString::fromUtf8("tabWidget_3"));
        tabWidget_3->setAutoFillBackground(false);
        tabWidget_3->setStyleSheet(QString::fromUtf8(""));
        tabWidget_3->setUsesScrollButtons(false);
        tabWidget_3->setDocumentMode(true);
        tabWidget_3->setTabsClosable(true);
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        verticalLayout_2 = new QVBoxLayout(tab_5);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new GigaOpenGLWidget(tab_5);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        sizePolicy1.setHeightForWidth(openGLWidget->sizePolicy().hasHeightForWidth());
        openGLWidget->setSizePolicy(sizePolicy1);
        openGLWidget->setMinimumSize(QSize(200, 200));

        verticalLayout_2->addWidget(openGLWidget);

        tabWidget_3->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        verticalLayout_5 = new QVBoxLayout(tab_6);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        plainTextEdit = new QPlainTextEdit(tab_6);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(1);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy3);

        verticalLayout_5->addWidget(plainTextEdit);

        tabWidget_3->addTab(tab_6, QString());

        openGLlayout->addWidget(tabWidget_3);


        horizontalLayout->addLayout(openGLlayout);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        propertiesFrame = new QFrame(centralWidget);
        propertiesFrame->setObjectName(QString::fromUtf8("propertiesFrame"));
        propertiesFrame->setMinimumSize(QSize(250, 0));
        propertiesFrame->setFrameShape(QFrame::NoFrame);
        propertiesFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_7 = new QVBoxLayout(propertiesFrame);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        propertiesLayout = new QVBoxLayout();
        propertiesLayout->setSpacing(6);
        propertiesLayout->setObjectName(QString::fromUtf8("propertiesLayout"));

        verticalLayout_7->addLayout(propertiesLayout);


        verticalLayout_4->addWidget(propertiesFrame);


        horizontalLayout->addLayout(verticalLayout_4);

        horizontalLayout->setStretch(1, 1);

        horizontalLayout_2->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 711, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuAdd = new QMenu(menuBar);
        menuAdd->setObjectName(QString::fromUtf8("menuAdd"));
        menuGame = new QMenu(menuBar);
        menuGame->setObjectName(QString::fromUtf8("menuGame"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuAdd->menuAction());
        menuBar->addAction(menuGame->menuAction());
        menuFile->addAction(actionNew_Project);
        menuFile->addAction(actionOpen_Project);
        menuFile->addAction(actionSave_Project);
        menuFile->addAction(actionClose_Project);
        menuFile->addSeparator();
        menuFile->addAction(actionNew_Scene);
        menuFile->addAction(actionOpen_Scene);
        menuFile->addAction(actionClose_Scene);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuAdd->addAction(actionEntity);
        menuAdd->addAction(actionComponent);
        menuAdd->addAction(actionScript);
        menuGame->addAction(actionBuild);
        menuGame->addAction(actionRun);
        menuGame->addAction(actionDeploy);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);
        btnEditMode_Grab->setDefault(false);
        tabWidget_2->setCurrentIndex(0);
        tabWidget_3->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "GIGA Editor", nullptr));
        actionNew_Project->setText(QCoreApplication::translate("MainWindow", "New Project...", nullptr));
        actionOpen_Project->setText(QCoreApplication::translate("MainWindow", "Open Project...", nullptr));
        actionClose_Project->setText(QCoreApplication::translate("MainWindow", "Close Project", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionNew_Scene->setText(QCoreApplication::translate("MainWindow", "New Scene...", nullptr));
        actionOpen_Scene->setText(QCoreApplication::translate("MainWindow", "Open Scene...", nullptr));
        actionClose_Scene->setText(QCoreApplication::translate("MainWindow", "Close Scene", nullptr));
        actionEntity->setText(QCoreApplication::translate("MainWindow", "Entity...", nullptr));
        actionComponent->setText(QCoreApplication::translate("MainWindow", "Component...", nullptr));
        actionSave_Project->setText(QCoreApplication::translate("MainWindow", "Save Project", nullptr));
        actionScript->setText(QCoreApplication::translate("MainWindow", "Script...", nullptr));
        actionBuild->setText(QCoreApplication::translate("MainWindow", "Build", nullptr));
        actionRun->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
        actionDeploy->setText(QCoreApplication::translate("MainWindow", "Deploy", nullptr));
        btnEditMode_Grab->setText(QString());
        btnEditMode_Move->setText(QString());
        btnEditMode_Rotate->setText(QString());
        btnEditMode_Scale->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Tools", nullptr));
        btnEditMode_TerrainAdd->setText(QString());
        btnEditMode_TerrainRaise->setText(QString());
        btnEditMode_TerrainFlatten->setText(QString());
        btnEditMode_TerrainPaint->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Terrain", nullptr));
        sceneSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "Search scene...", nullptr));
        btnNewEntity->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_3), QCoreApplication::translate("MainWindow", "Scene", nullptr));
        resourceSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "Search resources...", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_4), QCoreApplication::translate("MainWindow", "Resources", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_7), QCoreApplication::translate("MainWindow", "Brushes", nullptr));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_5), QCoreApplication::translate("MainWindow", "Scene", nullptr));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_6), QCoreApplication::translate("MainWindow", "Tab 2", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuAdd->setTitle(QCoreApplication::translate("MainWindow", "Create", nullptr));
        menuGame->setTitle(QCoreApplication::translate("MainWindow", "Game", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
