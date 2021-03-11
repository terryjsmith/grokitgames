QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += opengl openglwidgets

CONFIG += c++11

THIRDPARTY_DIR = "C:/Users/Smith/Documents/Projects/ThirdParty"
ENGINE_DIR = "C:/Users/Smith/Documents/Projects/grokitgames"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$THIRDPARTY_DIR/gl3w/src/gl3w.c \
    dialogcreateentity.cpp \
    editoropenglwidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    dialogcreateentity.h \
    editoropenglwidget.h \
    mainwindow.h \
    metatypes.h

FORMS += \
    dialogcreateentity.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$ENGINE_DIR/Source/Engine

INCLUDEPATH += $$THIRDPARTY_DIR/gl3w/include
INCLUDEPATH += $$THIRDPARTY_DIR/glm/include
INCLUDEPATH += $$THIRDPARTY_DIR/openssl/include
INCLUDEPATH += $$THIRDPARTY_DIR/soil/include
INCLUDEPATH += $THIRDPARTY_DIR/sqlite/include

# win32
win32: LIBS += -lWs2_32 -lopengl32 -lGdi32 -lUser32 -lWldap32 -lAdvapi32 -lOleAut32 -lOle32 -lWinmm -lVersion -lBcrypt

# core
unix|win32: LIBS += -L$$ENGINE_DIR/Bin/ -lcore

DEPENDPATH += $$ENGINE_DIR/Bin

win32: PRE_TARGETDEPS += $$ENGINE_DIR/Bin/core.lib
else:unix: PRE_TARGETDEPS += $$ENGINE_DIR/Bin/libcore.a

# Assimp
unix|win32: LIBS += -L$$THIRDPARTY_DIR/assimp/lib/ -lassimp
unix|win32: LIBS += -L$$THIRDPARTY_DIR/assimp/lib/ -lIrrXML
unix|win32: LIBS += -L$$THIRDPARTY_DIR/assimp/lib/ -lzlibstaticd

INCLUDEPATH += $$THIRDPARTY_DIR/assimp/include
DEPENDPATH += $$THIRDPARTY_DIR/assimp/include

win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/assimp/lib/assimp.lib
win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/assimp/lib/IrrXML.lib
win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/assimp/lib/zlibstaticd.lib

else:unix: PRE_TARGETDEPS += $$THIRDPARTY_DIR/assimp/lib/libassimp.a

# Bullet
unix|win32: LIBS += -L$$THIRDPARTY_DIR/bullet/lib/ -lBulletCollision_Debug
unix|win32: LIBS += -L$$THIRDPARTY_DIR/bullet/lib/ -lBulletDynamics_Debug
unix|win32: LIBS += -L$$THIRDPARTY_DIR/bullet/lib/ -lLinearMath_Debug

INCLUDEPATH += $$THIRDPARTY_DIR/bullet/include
DEPENDPATH += $$THIRDPARTY_DIR/bullet/include

win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/bullet/lib/BulletCollision_Debug.lib
win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/bullet/lib/BulletDynamics_Debug.lib
win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/bullet/lib/LinearMath_Debug.lib

else:unix: PRE_TARGETDEPS += $$THIRDPARTY_DIR/bullet/lib/libBulletCollision_Debug.a

# fmod
unix|win32: LIBS += -L$$THIRDPARTY_DIR/fmod/lib/ -lfmod64_vc

INCLUDEPATH += $$THIRDPARTY_DIR/fmod/include
DEPENDPATH += $$THIRDPARTY_DIR/fmod/include

# glfw
unix|win32: LIBS += -L$$THIRDPARTY_DIR/glfw/lib/ -lglfw3

INCLUDEPATH += $$THIRDPARTY_DIR/glfw/include
DEPENDPATH += $$THIRDPARTY_DIR/glfw/include

win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/glfw/lib/glfw3.lib
else:unix: PRE_TARGETDEPS += $$THIRDPARTY_DIR/glfw/lib/libglfw3.a

# libcurl
unix|win32: LIBS += -L$$THIRDPARTY_DIR/libcurl/lib/ -llibcurl-d

INCLUDEPATH += $$THIRDPARTY_DIR/libcurl/include
DEPENDPATH += $$THIRDPARTY_DIR/libcurl/include

win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/libcurl/lib/libcurl-d.lib
else:unix: PRE_TARGETDEPS += $$THIRDPARTY_DIR/libcurl/lib/liblibcurl-d.a

# mono
unix|win32: LIBS += -L$$THIRDPARTY_DIR/mono/lib/ -llibmono-static-sgen

INCLUDEPATH += $$THIRDPARTY_DIR/mono/include/mono-2.0
DEPENDPATH += $$THIRDPARTY_DIR/mono/include/mono-2.0

win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/mono/lib/libmono-static-sgen.lib
else:unix: PRE_TARGETDEPS += $$THIRDPARTY_DIR/mono/lib/liblibmono-static-sgen.a

# mysql
unix|win32: LIBS += -L$$THIRDPARTY_DIR/mysql/lib/ -llibmysql

INCLUDEPATH += $$THIRDPARTY_DIR/mysql/include
DEPENDPATH += $$THIRDPARTY_DIR/mysql/include

win32: PRE_TARGETDEPS += $$THIRDPARTY_DIR/mysql/lib/libmysql.lib
else:unix: PRE_TARGETDEPS += $$THIRDPARTY_DIR/mysql/lib/libmysql.a
