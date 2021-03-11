/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.0.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "editoropenglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Project;
    QAction *actionOpen_Project;
    QAction *actionClose_Project;
    QAction *actionExit;
    QAction *actionAdd_Entity;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTabWidget *mainTabWidget;
    QWidget *tabViewport;
    QVBoxLayout *verticalLayout_4;
    EditorOpenGLWidget *openGLWidget;
    QWidget *tabDoc;
    QHBoxLayout *horizontalLayout_7;
    QTextEdit *textEdit;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget_2;
    QWidget *tabScene;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *txtSearchScene;
    QPushButton *btnCreateEntity;
    QTreeView *viewScene;
    QWidget *tabTools;
    QVBoxLayout *verticalLayout_5;
    QFrame *frameTools;
    QWidget *tab;
    QVBoxLayout *verticalLayout_6;
    QFrame *frame_2;
    QGroupBox *groupBoxProperties;
    QVBoxLayout *verticalLayout_7;
    QFormLayout *layoutComponentProperties;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_4;
    QTabWidget *tabResources;
    QWidget *tab_4;
    QHBoxLayout *horizontalLayout_3;
    QListView *listViewResources;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuWorld;
    QMenu *menuView;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(794, 625);
        actionNew_Project = new QAction(MainWindow);
        actionNew_Project->setObjectName(QString::fromUtf8("actionNew_Project"));
        actionOpen_Project = new QAction(MainWindow);
        actionOpen_Project->setObjectName(QString::fromUtf8("actionOpen_Project"));
        actionClose_Project = new QAction(MainWindow);
        actionClose_Project->setObjectName(QString::fromUtf8("actionClose_Project"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAdd_Entity = new QAction(MainWindow);
        actionAdd_Entity->setObjectName(QString::fromUtf8("actionAdd_Entity"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout_2 = new QHBoxLayout(centralwidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mainTabWidget = new QTabWidget(centralwidget);
        mainTabWidget->setObjectName(QString::fromUtf8("mainTabWidget"));
        mainTabWidget->setDocumentMode(true);
        mainTabWidget->setTabsClosable(true);
        mainTabWidget->setMovable(true);
        tabViewport = new QWidget();
        tabViewport->setObjectName(QString::fromUtf8("tabViewport"));
        verticalLayout_4 = new QVBoxLayout(tabViewport);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        openGLWidget = new EditorOpenGLWidget(tabViewport);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));

        verticalLayout_4->addWidget(openGLWidget);

        mainTabWidget->addTab(tabViewport, QString());
        tabDoc = new QWidget();
        tabDoc->setObjectName(QString::fromUtf8("tabDoc"));
        horizontalLayout_7 = new QHBoxLayout(tabDoc);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        textEdit = new QTextEdit(tabDoc);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        horizontalLayout_7->addWidget(textEdit);

        mainTabWidget->addTab(tabDoc, QString());

        horizontalLayout->addWidget(mainTabWidget);

        frame_3 = new QFrame(centralwidget);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_3);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        tabWidget_2 = new QTabWidget(frame_3);
        tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget_2->sizePolicy().hasHeightForWidth());
        tabWidget_2->setSizePolicy(sizePolicy);
        tabWidget_2->setDocumentMode(false);
        tabScene = new QWidget();
        tabScene->setObjectName(QString::fromUtf8("tabScene"));
        verticalLayout_3 = new QVBoxLayout(tabScene);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setSizeConstraint(QLayout::SetFixedSize);
        horizontalLayout_8->setContentsMargins(-1, 0, -1, -1);
        txtSearchScene = new QLineEdit(tabScene);
        txtSearchScene->setObjectName(QString::fromUtf8("txtSearchScene"));

        horizontalLayout_8->addWidget(txtSearchScene);

        btnCreateEntity = new QPushButton(tabScene);
        btnCreateEntity->setObjectName(QString::fromUtf8("btnCreateEntity"));
        btnCreateEntity->setMaximumSize(QSize(30, 16777215));

        horizontalLayout_8->addWidget(btnCreateEntity);


        verticalLayout_3->addLayout(horizontalLayout_8);

        viewScene = new QTreeView(tabScene);
        viewScene->setObjectName(QString::fromUtf8("viewScene"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(viewScene->sizePolicy().hasHeightForWidth());
        viewScene->setSizePolicy(sizePolicy1);
        viewScene->setMinimumSize(QSize(0, 0));
        viewScene->setMaximumSize(QSize(250, 200));
        viewScene->setBaseSize(QSize(0, 0));
        viewScene->setFrameShape(QFrame::NoFrame);
        viewScene->setRootIsDecorated(false);

        verticalLayout_3->addWidget(viewScene);

        tabWidget_2->addTab(tabScene, QString());
        tabTools = new QWidget();
        tabTools->setObjectName(QString::fromUtf8("tabTools"));
        verticalLayout_5 = new QVBoxLayout(tabTools);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        frameTools = new QFrame(tabTools);
        frameTools->setObjectName(QString::fromUtf8("frameTools"));
        frameTools->setFrameShape(QFrame::StyledPanel);
        frameTools->setFrameShadow(QFrame::Raised);

        verticalLayout_5->addWidget(frameTools);

        tabWidget_2->addTab(tabTools, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_6 = new QVBoxLayout(tab);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        frame_2 = new QFrame(tab);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);

        verticalLayout_6->addWidget(frame_2);

        tabWidget_2->addTab(tab, QString());

        verticalLayout_2->addWidget(tabWidget_2);

        groupBoxProperties = new QGroupBox(frame_3);
        groupBoxProperties->setObjectName(QString::fromUtf8("groupBoxProperties"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBoxProperties->sizePolicy().hasHeightForWidth());
        groupBoxProperties->setSizePolicy(sizePolicy2);
        groupBoxProperties->setMaximumSize(QSize(255, 16777215));
        groupBoxProperties->setFlat(true);
        groupBoxProperties->setCheckable(false);
        groupBoxProperties->setChecked(false);
        verticalLayout_7 = new QVBoxLayout(groupBoxProperties);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 9, 0, 9);
        layoutComponentProperties = new QFormLayout();
        layoutComponentProperties->setObjectName(QString::fromUtf8("layoutComponentProperties"));
        layoutComponentProperties->setRowWrapPolicy(QFormLayout::WrapLongRows);

        verticalLayout_7->addLayout(layoutComponentProperties);


        verticalLayout_2->addWidget(groupBoxProperties);


        horizontalLayout->addWidget(frame_3);


        verticalLayout->addLayout(horizontalLayout);

        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy3);
        frame->setMinimumSize(QSize(0, 200));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        tabResources = new QTabWidget(frame);
        tabResources->setObjectName(QString::fromUtf8("tabResources"));
        tabResources->setTabPosition(QTabWidget::North);
        tabResources->setTabShape(QTabWidget::Rounded);
        tabResources->setTabBarAutoHide(false);
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        horizontalLayout_3 = new QHBoxLayout(tab_4);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(9, 9, 9, 9);
        listViewResources = new QListView(tab_4);
        listViewResources->setObjectName(QString::fromUtf8("listViewResources"));
        listViewResources->setFrameShape(QFrame::NoFrame);
        listViewResources->setIconSize(QSize(48, 48));
        listViewResources->setFlow(QListView::LeftToRight);
        listViewResources->setGridSize(QSize(80, 80));
        listViewResources->setViewMode(QListView::IconMode);
        listViewResources->setUniformItemSizes(false);
        listViewResources->setWordWrap(false);

        horizontalLayout_3->addWidget(listViewResources);

        tabResources->addTab(tab_4, QString());

        horizontalLayout_4->addWidget(tabResources);


        verticalLayout->addWidget(frame);


        horizontalLayout_2->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 794, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuWorld = new QMenu(menubar);
        menuWorld->setObjectName(QString::fromUtf8("menuWorld"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuWorld->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew_Project);
        menuFile->addAction(actionOpen_Project);
        menuFile->addAction(actionClose_Project);
        menuFile->addAction(actionExit);
        menuWorld->addAction(actionAdd_Entity);

        retranslateUi(MainWindow);

        mainTabWidget->setCurrentIndex(0);
        tabWidget_2->setCurrentIndex(0);
        tabResources->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionNew_Project->setText(QCoreApplication::translate("MainWindow", "New Project...", nullptr));
        actionOpen_Project->setText(QCoreApplication::translate("MainWindow", "Open Project...", nullptr));
        actionClose_Project->setText(QCoreApplication::translate("MainWindow", "Close Project", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAdd_Entity->setText(QCoreApplication::translate("MainWindow", "Create Entity...", nullptr));
        mainTabWidget->setTabText(mainTabWidget->indexOf(tabViewport), QCoreApplication::translate("MainWindow", "Viewport", nullptr));
        mainTabWidget->setTabText(mainTabWidget->indexOf(tabDoc), QCoreApplication::translate("MainWindow", "Tab 2", nullptr));
        btnCreateEntity->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tabScene), QCoreApplication::translate("MainWindow", "Scene", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tabTools), QCoreApplication::translate("MainWindow", "Tools", nullptr));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab), QCoreApplication::translate("MainWindow", "Build", nullptr));
        groupBoxProperties->setTitle(QCoreApplication::translate("MainWindow", "Properties", nullptr));
        tabResources->setTabText(tabResources->indexOf(tab_4), QCoreApplication::translate("MainWindow", "Resources", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
        menuWorld->setTitle(QCoreApplication::translate("MainWindow", "World", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
