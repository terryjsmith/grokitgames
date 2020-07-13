#include "mainwindow.h"
#include "editorsettings.h"
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

    // Open error log
    LogSystem* logSystem = GetSystem<LogSystem>();
    logSystem->Open("error.log");

    // Set up resource paths
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    resourceSystem->AddSearchPath("Resources/Shaders");
    resourceSystem->AddSearchPath("Resources/Scripts");
    resourceSystem->AddSearchPath("Resources/Models");

    // Register built-in component types
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    metaSystem->RegisterComponentType("TransformComponent");
    metaSystem->RegisterComponentType("AudioComponent");
    metaSystem->RegisterComponentType("RigidBodyComponent");
    metaSystem->RegisterComponentType("CameraComponent");
    metaSystem->RegisterComponentType("DirectionalLightComponent");
    metaSystem->RegisterComponentType("MeshComponent");
    metaSystem->RegisterComponentType("PointLightComponent");
    metaSystem->RegisterComponentType("Skybox");
    metaSystem->RegisterComponentType("TerrainComponent");
    metaSystem->RegisterComponentType("ScriptComponent");
    metaSystem->RegisterComponentType("BillboardComponent");
    metaSystem->RegisterComponentType("ParticleEmitterComponent");

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

    // Initialize settings
    EditorSettings* settings = EditorSettings::GetInstance();

    // Load editor files
    scriptingSystem->LoadLibrary("Resources/Scripting/GIGAEditor.dll");

    return a.exec();
}
