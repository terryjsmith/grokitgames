
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
#include <Render/GuiSystem.h>

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
    GuiSystem* guiSystem = app->CreateSystem<GuiSystem>();
    
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
    
    // Initialize GUI
    guiSystem->Startup();
    
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
    
    // Set up render pipeline
    DeferredRenderPipeline* renderPipeline = new DeferredRenderPipeline();
    renderPipeline->Initialize(framebufferWidth, framebufferHeight);
    
    renderSystem->SetRenderPipeline(renderPipeline);
    
    // Initialize keyboard
    Keyboard* keyboard = new Keyboard();
    keyboard->Initialize();
    
    inputSystem->RegisterInputDevice(keyboard);
    
    // Game timer
    Timer* gameTimer = new Timer();
    gameTimer->Start();

    // Main loop
    while(window->IsClosing() == false) {
        PROFILE_START_FRAME();
        
        float delta = gameTimer->Duration();
        gameTimer->Reset();
        
        // Start gui frame
        guiSystem->StartFrame();
        
        app->Update(delta);
        
        PROFILE_START_AREA("ProcessEvents");
        window->ProcessEvents();
        PROFILE_END_AREA("ProcessEvents");
        
        PROFILE_START_AREA("ImGui");
        
        // Render gui
        bool closed = false;
        
        int windowWidth, windowHeight;
        window->GetWindowDimensions(windowWidth, windowHeight);
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);
        
        // Start window
        ImGui::Begin("main_window", &closed, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(255, 0, 0, 255));
        ImGui::BeginChild("resource_panel", ImVec2(200, 0), false, ImGuiWindowFlags_NoDecoration);
        
        // Resource panel
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 255, 0, 255));
        ImGui::BeginChild("render_panel", ImVec2(windowWidth - 400 - (ImGui::GetStyle().ItemSpacing.x * 4), 0), false, ImGuiWindowFlags_NoDecoration);
        
        // Render panel
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 255, 255));
        ImGui::BeginChild("scene_panel", ImVec2(200, 0), false, ImGuiWindowFlags_NoDecoration);
        
        // Scene objects panel
        
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        ImGui::End();
        
        guiSystem->Render();
        PROFILE_END_AREA("ImGui");
        
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
