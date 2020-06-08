
#include <iostream>
#include <Core/MetaSystem.h>
#include <Core/Application.h>
#include <Core/World.h>
#include <Meta/MetaData.h>
#include <IO/ResourceSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/TimeSystem.h>
#include <Render/RenderSystem.h>
#include <IO/Window.h>
#include <Render/OpenGL/OpenGLRenderSystem.h>
#include <Render/Pipelines/DeferredRenderPipeline.h>
#include <Render/Shape.h>
#include <IO/InputSystem.h>
#include <IO/Keyboard.h>
#include <IO/Command.h>
#include <Core/MessageSystem.h>
#include <Core/Timer.h>
#include <Assets/AssimpImporter.h>
#include <Render/MeshComponent.h>
#include <Render/PointLightComponent.h>
#include <Render/DirectionalLightComponent.h>
#include <Render/MaterialSystem.h>
#include <Render/TerrainSystem.h>
#include <Render/Skybox.h>
#include <Scripting/ScriptingSystem.h>
#include <Network/NetworkSystem.h>
#include <Audio/AudioSystem.h>
#include <Render/AnimationSystem.h>
#include <IO/LogSystem.h>
#include <IO/Profiler.h>
#include "register_globals.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";

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
    
    // Initialize systems
    app->Initialize();
    
    // Open error log
    logSystem->Open("error.log");
    
    // Create window
    Window* window = Window::GetInstance();
    window->Create("Game", 800, 600, false);
    
    // Get framebuffer size (different for retina)
    int framebufferWidth, framebufferHeight;
    window->GetFramebufferDimensions(framebufferWidth, framebufferHeight);
    
    // Initialize render system
    renderSystem->Initialize(framebufferWidth, framebufferHeight, false);
    
    // Set up resource paths
    resourceSystem->AddSearchPath("Resources/Shaders");
    resourceSystem->AddSearchPath("Resources/Models");
    resourceSystem->AddSearchPath("Resources/Textures");
    resourceSystem->AddSearchPath("Resources/Terrain");
    resourceSystem->AddSearchPath("Resources/Scripts");
    resourceSystem->AddSearchPath("Resources/Audio");
    
    // Register meta data
    MetaData::RegisterMetaFunctions();
    
    // Initialize the world
    World* world = World::GetInstance();
    
    /* Create a shape
    Shape* shape = new Shape();
    
    float vertices[] = {
        -1.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
         0.0f,  2.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };
    
    VertexFormat* fmt = renderSystem->CreateVertexFormat();
    fmt->Initialize();
    fmt->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 3, 0);
    fmt->AddVertexAttrib(VERTEXTYPE_ATTRIB_COLOR, 3, 3);
    
    shape->SetVertices(fmt, vertices, 3);
    
    Entity* triangle = world->CreateEntity();
    triangle->AddComponent(shape);*/
    
    // Set up camera
    Entity* playerEntity = world->CreateEntity();
    CameraComponent* camera = playerEntity->CreateComponent<CameraComponent>();
    
    camera->transform->SetWorldPosition(vector3(0, 2.0f, 10.0f));
    //camera->transform->Rotate(vector3(0, 1, 0), -80);
    Scene* scene = renderSystem->GetScene();
    
    scene->camera = camera;
    
    // Set up render pipeline
    DeferredRenderPipeline* renderPipeline = new DeferredRenderPipeline();
    renderPipeline->Initialize(framebufferWidth, framebufferHeight);
    
    renderSystem->SetRenderPipeline(renderPipeline);
    
    // Initialize keyboard
    Keyboard* keyboard = new Keyboard();
    keyboard->Initialize();
    
    inputSystem->RegisterInputDevice(keyboard);
    
    // Set up scripting globals
    RegisterGlobals();
    
    // Game timer
    Timer* gameTimer = new Timer();
    gameTimer->Start();
    
    /*AssimpImporter* importer = new AssimpImporter();
    std::string crateFilename = resourceSystem->FindResourcePath("jasper.fbx");
    Mesh* mesh = importer->LoadMesh(crateFilename);
    importer->LoadAnimation(resourceSystem->FindResourcePath("Walking.fbx"), "Walk", mesh);
    importer->LoadAnimation(resourceSystem->FindResourcePath("BreathingIdle.fbx"), "Idle", mesh);
    importer->LoadAnimation(resourceSystem->FindResourcePath("Running.fbx"), "Run", mesh);
    
    Entity* crateEntity = world->CreateEntity();
    crateEntity->name = "Jasper";
    MeshComponent* mc = crateEntity->CreateComponent<MeshComponent>();
    //mc->transform->Rotate(vector3(1, 0, 0), -90);
    //mc->transform->Rotate(vector3(0, 1, 0), 180);
    mc->transform->Scale(vector3(0.003f));
    mc->Initialize(mesh);
    
    ScriptComponent* sc = playerEntity->CreateComponent<ScriptComponent>();
    Script* script = dynamic_cast<Script*>(resourceSystem->LoadResource("game.js", "Script"));
    sc->Initialize(script);
    
    Entity* floor = world->CreateEntity();
    std::string floorFilename = resourceSystem->FindResourcePath("floor.fbx");
    Mesh* floorMesh = importer->LoadMesh(floorFilename);
    
    MeshComponent* fmc = floor->CreateComponent<MeshComponent>();
    fmc->Initialize(floorMesh);
    fmc->transform->Rotate(vector3(1, 0, 0), -90);
    
    Entity* light = world->CreateEntity();
    light->name = "Light";
    PointLightComponent* plc = light->CreateComponent<PointLightComponent>();
    plc->transform->SetWorldPosition(vector3(3, 3, 3));
    plc->SetAttenuation(15.0f);
    plc->Initialize();
    
    Entity* sun = world->CreateEntity();
    DirectionalLightComponent* dlc = sun->CreateComponent<DirectionalLightComponent>();
    dlc->transform->SetWorldPosition(glm::normalize(vector3(-0.4, -0.7, 0.4)));
    dlc->Initialize();
    
    //scene->camera = dlc->GetCamera(2);
    
    renderSystem->SetAmbientLighting(vector3(0.7f, 0.7f, 0.7f));
    
    Entity* terrain = world->CreateEntity();
    TerrainComponent* tc = terrain->CreateComponent<TerrainComponent>();
    tc->Load("terrain1.json", vector3(0));
    
    Entity* skybox = world->CreateEntity();
    Skybox* sb = skybox->CreateComponent<Skybox>();
    sb->Initialize("sky");*/
    
    // Main loop
    while(window->IsClosing() == false) {
        PROFILE_START_FRAME();
        
        float delta = gameTimer->Duration();
        gameTimer->Reset();
        
        app->Update(delta);
        
        PROFILE_START_AREA("ProcessEvents");
        window->ProcessEvents();
        PROFILE_END_AREA("ProcessEvents");
        
        PROFILE_START_AREA("SwapBuffer");
        window->SwapBuffer();
        PROFILE_END_AREA("SwapBuffer");
        
        PROFILE_END_FRAME();
        timeSystem->Sleep(1);
    }
    
    // Close window
    delete window;
    
    // Close log
    Profiler::GetInstance()->Log();
    logSystem->Close();
    
    return 0;
}
