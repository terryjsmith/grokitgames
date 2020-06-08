#-------------------------------------------------
#
# Project created by QtCreator 2019-08-28T22:13:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Editor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        gigaopenglwidget.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        gigaopenglwidget.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -L$$PWD/../../../Bin/ -lcore
unix|win32: LIBS += -L$$PWD/../../../Bin/ -lio
unix|win32: LIBS += -L$$PWD/../../../Bin/ -lrender
unix|win32: LIBS += -L$$PWD/../../../Bin/ -lphysics
unix|win32: LIBS += -L$$PWD/../../../Bin/ -laudio
unix|win32: LIBS += -L$$PWD/../../../Bin/ -lscripting
unix|win32: LIBS += -L$$PWD/../../../Bin/ -lmeta
unix|win32: LIBS += -L$$PWD/../../../Bin/ -lnetworking

INCLUDEPATH += $$PWD/../../Engine
DEPENDPATH += $$PWD/../../../Bin

INCLUDEPATH += $$PWD/../../../../ThirdParty/assimp/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/bullet/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/fmod/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/gl3w/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/glm/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/rapidjson/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/libcurl/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/mysql/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/openssl/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/soil/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/sqlite/include
INCLUDEPATH += $$PWD/../../../../ThirdParty/v8/include
