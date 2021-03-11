#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

#include <Core/Application.h>
#include <Core/MetaSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/TimeSystem.h>
#include <Core/MessageSystem.h>
#include <IO/ResourceSystem.h>
#include <IO/LogSystem.h>
#include <IO/InputSystem.h>
#include <Render/OpenGL/OpenGLRenderSystem.h>
#include <Render/TerrainSystem.h>
#include <Render/MaterialSystem.h>
#include <Render/AnimationSystem.h>
#include <Render/ParticleSystem.h>
#include <Audio/AudioSystem.h>
#include <Scripting/ScriptingSystem.h>
#include <Network/NetworkSystem.h>
#include <Network/ReplicationSystem.h>
#include <Meta/MetaData.h>
#include <Core/World.h>

int main(int argc, char *argv[])
{
    // Get application instance
    Application* app = Application::GetInstance();

    // Create systems
    MetaSystem* metaSystem = app->CreateSystem<MetaSystem>();
    ResourceSystem* resourceSystem = app->CreateSystem<ResourceSystem>();
    ErrorSystem* errorSystem = app->CreateSystem<ErrorSystem>();
    TerrainSystem* terrainSystem = app->CreateSystem<TerrainSystem>();
    RenderSystem* renderSystem = app->CreateSystem<OpenGLRenderSystem>();
    MessageSystem* messageSystem = app->CreateSystem<MessageSystem>();
    TimeSystem* timeSystem = app->CreateSystem<TimeSystem>();
    InputSystem* inputSystem = app->CreateSystem<InputSystem>();
    MaterialSystem* materialSystem = app->CreateSystem<MaterialSystem>();
    ScriptingSystem* scriptingSystem = app->CreateSystem<ScriptingSystem>();
    NetworkSystem* networkSystem = app->CreateSystem<NetworkSystem>();
    AudioSystem* audioSystem = app->CreateSystem<AudioSystem>();
    AnimationSystem* animationSystem = app->CreateSystem<AnimationSystem>();
    LogSystem* logSystem = app->CreateSystem<LogSystem>();
    ParticleSystem* particleSystem = app->CreateSystem<ParticleSystem>();
    ReplicationSystem* replicationSystem = app->CreateSystem<ReplicationSystem>();

    // Initialize systems
    app->Initialize();

    // Open error log
    logSystem->Open("error.log");

    // Register meta data
    MetaData::RegisterMetaFunctions();

    // Initialize the world
    World* world = World::GetInstance();

    // Start scripting system
    scriptingSystem->Start();

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

    MainWindow* w = MainWindow::GetInstance();
    w->showMaximized();

    return a.exec();
}
