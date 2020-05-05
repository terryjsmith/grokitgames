
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
#include "register_globals.h"

Resource* m_errorLog = 0;
int moveFactor = 0;
int turnFactor = 0;
DirectionalLightComponent* dirLightCo = 0;

enum Commands {
    MOVE_FORWARD = 1000,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT
};

void HandleError(Error* err) {
    TimeSystem* timeSystem = GetSystem<TimeSystem>();
    tm timestamp;
    timeSystem->GetTime(&timestamp);
    
    char timestr[200];
    strftime(timestr, 200, "%m/%d/%Y %I:%M:%S %p", &timestamp);
    
    std::string errstr;
    switch(err->level) {
        case Error::ERROR_INFO:
            errstr = "INFO ";
            break;
        case Error::ERROR_DEBUG:
            errstr = "DEBUG";
            break;
        case Error::ERROR_WARN:
            errstr = "WARN ";
            break;
        case Error::ERROR_FATAL:
            errstr = "FATAL";
            break;
        default: break;
    }
    
    char output[1000];
    sprintf(output, "[%s] [%s] %s %s", timestr, errstr.c_str(), err->message.c_str(), err->detail.c_str());
    m_errorLog->WriteLine(output);
    
    // Do other stuff here later, message boxes and stuff
}

void CommandHandler(GigaObject* obj, Message* message) {
    Command* cmd = (Command*)message->obj;
    if(cmd->GetState() == Command::COMMAND_START) {
        if(cmd->GetType() == MOVE_FORWARD) {
            moveFactor = 1;
        }
        
        if(cmd->GetType() == MOVE_BACKWARD) {
            moveFactor = -1;
        }
        
        if(cmd->GetType() == TURN_LEFT) {
            turnFactor = 1;
        }
        
        if(cmd->GetType() == TURN_RIGHT) {
            turnFactor = -1;
        }
        if(cmd->GetType() == 100) {
            dirLightCo->GetDepthTexture(0)->Save("depth.bmp");
            dirLightCo->GetDepthTexture(1)->Save("depth1.bmp");
            dirLightCo->GetDepthTexture(2)->Save("depth2.bmp");
            dirLightCo->GetDepthTexture(3)->Save("depth3.bmp");
        }
    }
    else {
        if(cmd->GetType() == MOVE_FORWARD) {
            moveFactor = 0;
        }
        
        if(cmd->GetType() == MOVE_BACKWARD) {
            moveFactor = 0;
        }
        
        if(cmd->GetType() == TURN_LEFT) {
            turnFactor = 0;
        }
        
        if(cmd->GetType() == TURN_RIGHT) {
            turnFactor = 0;
        }
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";

    // Get application instance
    Application* app = Application::GetInstance();
    
    // Error log
    m_errorLog = new Resource();
    m_errorLog->Initialize("error.log", Resource::FILEMODE_APPEND);
    
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
    
    errorSystem->RegisterErrorHandler(Error::ERROR_DEBUG, HandleError);
    errorSystem->RegisterErrorHandler(Error::ERROR_INFO, HandleError);
    errorSystem->RegisterErrorHandler(Error::ERROR_WARN, HandleError);
    errorSystem->RegisterErrorHandler(Error::ERROR_FATAL, HandleError);
    
    // Initialize systems
    app->Initialize();
    
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
    
    // Register meta data
    MetaData::RegisterMetaFunctions();
    
    // Initialize the world
    World* world = World::GetInstance();
    
    // Set some globals
    scriptingSystem->SetGlobal("MOVE_FORWARD", new Variant(MOVE_FORWARD));
    scriptingSystem->SetGlobal("MOVE_BACKWARD", new Variant(MOVE_BACKWARD));
    scriptingSystem->SetGlobal("TURN_LEFT", new Variant(TURN_LEFT));
    scriptingSystem->SetGlobal("TURN_RIGHT", new Variant(TURN_RIGHT));
    
    // Create a shape
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
    triangle->AddComponent(shape);
    
    // Set up camera
    CameraComponent* camera = new CameraComponent();
    camera->transform->SetWorldPosition(vector3(100, 70.0f, 5));
    camera->transform->Rotate(vector3(0, 1, 0), -80);
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
    
    // Register keys
    Command::RegisterCommandType("MOVE_FORWARD", MOVE_FORWARD);
    Command::RegisterCommandType("MOVE_BACKWARD", MOVE_BACKWARD);
    Command::RegisterCommandType("TURN_LEFT", TURN_LEFT);
    Command::RegisterCommandType("TURN_RIGHT", TURN_RIGHT);
    Command::RegisterCommandType("TAKE_PICTURE", 100);
    
    inputSystem->RegisterInputMapping(keyboard, KEY_UP, "MOVE_FORWARD");
    inputSystem->RegisterInputMapping(keyboard, KEY_DOWN, "MOVE_BACKWARD");
    inputSystem->RegisterInputMapping(keyboard, KEY_LEFT, "TURN_LEFT");
    inputSystem->RegisterInputMapping(keyboard, KEY_RIGHT, "TURN_RIGHT");
    inputSystem->RegisterInputMapping(keyboard, KEY_P, "TAKE_PICTURE");
    
    messageSystem->RegisterCallback(0, "COMMAND_START", CommandHandler);
    messageSystem->RegisterCallback(0, "COMMAND_END", CommandHandler);
    
    // Set up scripting globals
    RegisterGlobals();
    
    // Game timer
    Timer* gameTimer = new Timer();
    gameTimer->Start();
    
    AssimpImporter* importer = new AssimpImporter();
    std::string crateFilename = resourceSystem->FindResourcePath("CommonTree_1.fbx");
    Mesh* mesh = importer->LoadFromFile(crateFilename);
    
    Entity* crateEntity = world->CreateEntity();
    
    MeshComponent* mc = crateEntity->CreateComponent<MeshComponent>();
    mc->Initialize(mesh);
    
    ScriptComponent* sc = crateEntity->CreateComponent<ScriptComponent>();
    Script* script = dynamic_cast<Script*>(resourceSystem->LoadResource("game.js", "Script"));
    sc->Initialize(script);
    
    std::string floorFilename = resourceSystem->FindResourcePath("floor.fbx");
    Mesh* floorMesh = importer->LoadFromFile(floorFilename);
    
    MeshComponent* fmc = new MeshComponent();
    fmc->Initialize(floorMesh);
    
    Entity* floorEntity = world->CreateEntity();
    floorEntity->AddComponent(fmc);
    
    PointLightComponent* plc = new PointLightComponent();
    plc->transform->SetWorldPosition(vector3(3, 3, 0));
    plc->SetAttenuation(10.0f);
    plc->Initialize();
    
    Entity* lightEntity = world->CreateEntity();
    //lightEntity->AddComponent(plc);
    
    DirectionalLightComponent* dlc = new DirectionalLightComponent();
    dlc->transform->SetWorldPosition(glm::normalize(vector3(-0.4, -0.7, 0.4)));
    dlc->Initialize();
    
    dirLightCo = dlc;
    
    //scene->camera = dlc->GetCamera(2);
    
    Entity* sunlight = world->CreateEntity();
    sunlight->AddComponent(dlc);
    
    renderSystem->SetAmbientLighting(vector3(0.7f, 0.7f, 0.7f));
    
    Entity* terrain = world->CreateEntity();
    TerrainComponent* tc = terrain->CreateComponent<TerrainComponent>();
    tc->Load("terrain1.json", vector3(0));
    
    Entity* skybox = world->CreateEntity();
    Skybox* sb = skybox->CreateComponent<Skybox>();
    sb->Initialize("sky");
    
    // Main loop
    while(window->IsClosing() == false) {
        float delta = gameTimer->Duration();
        gameTimer->Reset();
        
        camera->transform->Move(camera->transform->GetLook() * 15.0f * (float)moveFactor * delta);
        camera->transform->Rotate(vector3(0, 1, 0), 60.0f * (float)turnFactor * delta);
        app->Update(delta);
        
        window->ProcessEvents();
        window->SwapBuffer();
        
        timeSystem->Sleep(1);
    }
    
    // Close window
    delete window;
    
    // Close log
    m_errorLog->Close();
    delete m_errorLog;
    
    return 0;
}
