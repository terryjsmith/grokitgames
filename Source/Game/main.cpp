
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
#include <IO/Keyboard.h>
#include <IO/InputSystem.h>
#include <Core/MessageSystem.h>
#include <Core/Timer.h>
#include <Render/MaterialSystem.h>
#include <Scripting/ScriptingSystem.h>
#include <Network/NetworkSystem.h>
#include <Audio/AudioSystem.h>
#include <Render/AnimationSystem.h>
#include <Render/TerrainSystem.h>
#include <IO/LogSystem.h>
#include <IO/Profiler.h>
#include <IO/SQLiteDataLoader.h>
#include <Core/TransformComponent.h>
#include <Render/ParticleEmitterComponent.h>
#include <Render/ParticleSystem.h>
#include <Network/ReplicationSystem.h>
#include <Render/Defines.h>
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
    ParticleSystem* particleSystem = app->CreateSystem<ParticleSystem>();
    ReplicationSystem* replicationSystem = app->CreateSystem<ReplicationSystem>();
    
    // Initialize systems
    app->Initialize();
    
    // Open error log
    logSystem->Open("error.log");
    
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
    
    // Start scripting system
    scriptingSystem->Start();
    
    // Create window
    Window* window = Window::GetInstance();
    window->Create("Game", 800, 600, false);
    
    // Get framebuffer size (different for retina)
    int framebufferWidth, framebufferHeight;
    window->GetFramebufferDimensions(framebufferWidth, framebufferHeight);
    
    // Initialize render system
    renderSystem->Initialize(framebufferWidth, framebufferHeight, false);
    
    // Register built-in component types
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
    
    // Load game code
    if(Resource::Exists("game.dll")) {
        scriptingSystem->LoadLibrary("game.dll");
    }
    
    // Open our game database
    SQLiteDataLoader* dataLoader = new SQLiteDataLoader();
    dataLoader->Open("game.db");
    
    /* Load entities
    Array<GigaObject*> entities = dataLoader->GetObjects("Entity");

    // Add entities to world
    auto it = entities.begin();
    for(; it != entities.end(); it++) {
        Entity* entity = dynamic_cast<Entity*>(*it);
        world->AddEntity(entity);
    }

    // Load components
    Array<std::string> componentTypes = metaSystem->GetComponentTypes();
    auto cti = componentTypes.begin();
    for(; cti != componentTypes.end(); cti++) {
        Array<GigaObject*> components = dataLoader->GetObjects(*cti);
        auto ci = components.begin();
        for(; ci != components.end(); ci++) {
            // Check whether this is attached to an entity
            Component* component = dynamic_cast<Component*>(*ci);
            Entity* entity = component->GetParent();
            if(entity != 0) {

            }
        }
    }*/

    // Start client
    scriptingSystem->StartClient();
    
    // Set up render pipeline
    DeferredRenderPipeline* renderPipeline = new DeferredRenderPipeline();
    renderPipeline->Initialize(0, 0, framebufferWidth, framebufferHeight);
    
    renderSystem->SetRenderPipeline(renderPipeline);
    
    // Initialize keyboard
    Keyboard* keyboard = new Keyboard();
    keyboard->Initialize();
    
    inputSystem->RegisterInputDevice(keyboard);
    
    /**
     * Load extra stuff
     */
    /*
    Entity* entity = world->CreateEntity();
    ParticleEmitterComponent* emitter = entity->CreateComponent<ParticleEmitterComponent>();
    emitter->GetTransform()->SetWorldPosition(vector3(0, 0.6f, 0));
    Texture2D* tex = (Texture2D*)resourceSystem->LoadResource("Resources/Textures/fireparticle.png", "Texture2D");
    emitter->Initialize(tex, 1.0f, 1000);
    
    ScriptComponent* script = entity->CreateComponent<ScriptComponent>();
    script->className = "Fire";
    */
    
    /*BillboardComponent* bc = entity->CreateComponent<BillboardComponent>();
    bc->Initialize();
    bc->GetTransform()->SetWorldPosition(vector3(0, 0.6f, 0));
    
    Texture2D* tex = (Texture2D*)resourceSystem->LoadResource("Resources/Textures/fireparticle.png", "Texture2D");
    bc->Create(tex, 0.5f);*/
    
    
    
    // Game timer
    Timer* gameTimer = new Timer();
    gameTimer->Start();

    // Main loop
    while(window->IsClosing() == false) {
        PROFILE_START_FRAME();
        
        float delta = gameTimer->Duration();
        gameTimer->Reset();
        
        // Clear full screen
        renderSystem->UseDefaultFramebuffer();
        renderSystem->SetViewport(framebufferWidth, framebufferHeight);
        renderSystem->SetClearColor(vector4(0, 0, 0, 1));
        renderSystem->Clear(COLOR_BUFFER_BIT);
        
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
