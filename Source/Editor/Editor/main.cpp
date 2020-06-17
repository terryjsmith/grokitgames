#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

#include <Core/MetaSystem.h>
#include <IO/ResourceSystem.h>
#include <Core/ErrorSystem.h>
#include <Render/TerrainSystem.h>
#include <Core/MessageSystem.h>
#include <Core/TimeSystem.h>
#include <IO/InputSystem.h>
#include <Render/MaterialSystem.h>
#include <Scripting/ScriptingSystem.h>
#include <Network/NetworkSystem.h>
#include <Audio/AudioSystem.h>
#include <Render/AnimationSystem.h>
#include <IO/LogSystem.h>
#include <Render/OpenGL/OpenGLRenderSystem.h>
#include <Meta/MetaData.h>
#include <Core/Application.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Grok It Games");
    QCoreApplication::setOrganizationDomain("grokitgames.com");
    QCoreApplication::setApplicationName("GIGA Editor");

    // Get application instance
    Application* app = Application::GetInstance();

    // Create systems
    app->CreateSystem<MetaSystem>();
    app->CreateSystem<ResourceSystem>();
    app->CreateSystem<ErrorSystem>();
    app->CreateSystem<TerrainSystem>();
    app->CreateSystem<OpenGLRenderSystem>();
    app->CreateSystem<MessageSystem>();
    app->CreateSystem<TimeSystem>();
    app->CreateSystem<InputSystem>();
    app->CreateSystem<MaterialSystem>();
    app->CreateSystem<ScriptingSystem>();
    app->CreateSystem<NetworkSystem>();
    app->CreateSystem<AudioSystem>();
    app->CreateSystem<AnimationSystem>();
    app->CreateSystem<LogSystem>();

    app->Initialize();

    // Set up resource paths
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    resourceSystem->AddSearchPath("Resources/Shaders");
    resourceSystem->AddSearchPath("Resources/Scripts");

    // Start up scripting system
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    scriptingSystem->Start();

    // Register meta data
    MetaData::RegisterMetaFunctions();

    QSurfaceFormat glFormat;
        glFormat.setVersion(4, 0);
        glFormat.setSamples(4);
        glFormat.setDepthBufferSize(24);
        glFormat.setStencilBufferSize(8);
        glFormat.setSwapInterval(0);
        glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        glFormat.setProfile( QSurfaceFormat::CoreProfile ); // Requires >=Qt-4.8.0
        QSurfaceFormat::setDefaultFormat(glFormat);

        QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
        QApplication a(argc, argv);

    MainWindow* w = MainWindow::getInstance();
    w->show();

    return a.exec();
}
