
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
#include <Render/Defines.h>

#include "IconsFontAwesome5.h"

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
    int windowWidth = 800;
    int windowHeight = 600;
    Window* window = Window::GetInstance();
    window->Create("Game", windowWidth, windowHeight, false, true);
    
    // Get framebuffer size (different for retina)
    int framebufferWidth, framebufferHeight;
    window->GetFramebufferDimensions(framebufferWidth, framebufferHeight);
    
    // Initialize render system
    renderSystem->Initialize(framebufferWidth, framebufferHeight, false);
    
    // Check for retina
    window->GetWindowDimensions(windowWidth, windowHeight);
    
    float retina = framebufferWidth / windowWidth;
    
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
    renderPipeline->Initialize(0, 0, framebufferWidth, framebufferHeight);
    
    renderSystem->SetRenderPipeline(renderPipeline);
    
    // Initialize keyboard
    Keyboard* keyboard = new Keyboard();
    keyboard->Initialize();
    
    inputSystem->RegisterInputDevice(keyboard);
    
    // Game timer
    Timer* gameTimer = new Timer();
    gameTimer->Start();
    
    // Load fonts
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
     
    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/" FONT_ICON_FILE_NAME_FAS, 12.0f, &icons_config, icons_ranges);
    
    ImVec2 lastWindowSize = ImVec2(0.0f, 0.0f);
    
    // Main loop
    while(window->IsClosing() == false) {
        PROFILE_START_FRAME();
        
        float delta = gameTimer->Duration();
        gameTimer->Reset();
        
        // Start gui frame
        guiSystem->StartFrame();
        
        // Clear full screen
        renderSystem->UseDefaultFramebuffer();
        renderSystem->SetViewport(framebufferWidth, framebufferHeight);
        renderSystem->SetClearColor(vector4(0, 0, 0, 1));
        renderSystem->Clear(COLOR_BUFFER_BIT);
        
        // Adjust viewport for render
        renderSystem->SetClearColor(vector4(1, 0, 0, 1));
        //renderSystem->SetViewport(lastWindowSize.x, lastWindowSize.y);
        //renderSystem->Clear(COLOR_BUFFER_BIT);
        
        // Do main game loop
        PROFILE_START_AREA("Update");
        app->Update(delta);
        PROFILE_END_AREA("Update");
        
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
        ImGui::Begin("main_window", &closed, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar);
        
        // Menu items
        bool menu_file_new_project = false;
        bool menu_file_open_project = false;
        bool menu_file_close_project = false;
        bool menu_file_publish = false;
        bool menu_file_exit = false;
        
        ImGui::BeginMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("New Project", NULL, &menu_file_new_project);
            ImGui::MenuItem("Open Project...", NULL, &menu_file_open_project);
            ImGui::MenuItem("Close Project", NULL, &menu_file_close_project, false);
            ImGui::Separator();
            ImGui::MenuItem("Publish...", NULL, &menu_file_publish, false);
            ImGui::Separator();
            ImGui::MenuItem("Exit", NULL, &menu_file_exit, false);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
        
        ImGui::BeginChild("resource_panel", ImVec2(200, 0), false, ImGuiWindowFlags_NoDecoration);
        
        // Resource panel
        ImGui::BeginChild("scene_entities", ImVec2(200, (windowHeight / 2.0f) - ImGui::GetStyle().ItemSpacing.y), false, ImGuiWindowFlags_NoDecoration);
        
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("entity_tab_bar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Scene"))
            {
                char search_buffer[200];
                memset(search_buffer, 0, 200);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));
                ImGui::InputText("", search_buffer, 200);
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::Text(ICON_FA_SEARCH);
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_PLUS))
                    ;
                
                int selected = -1;
                ImGui::BeginChild("resource_panel", ImVec2(200, 0), false, 0);
                ImGui::SetScrollY(0.0f);
                for(int i = 0; i < 40; i++)
                {
                    char buf[32];
                    sprintf(buf, "Object %d", i);
                    if (ImGui::Selectable(buf, selected == i))
                        ;
                }
                ImGui::EndChild();
                /*int selected = -1;
                if (ImGui::TreeNode("Entities"))
                {
                    for(int i = 0; i < 40; i++) {
                        char buf[32];
                        sprintf(buf, "Object %d", i);
                        if (ImGui::Selectable(buf, selected == i))
                            ;
                    }
                }
                ImGui::TreePop();*/
                
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        
        ImGui::EndChild();
        
        ImGui::BeginChild("scene_filetree", ImVec2(200, (windowHeight / 2.0f) - ImGui::GetStyle().ItemSpacing.y), false, ImGuiWindowFlags_NoDecoration);
        
        // File tree
        if (ImGui::BeginTabBar("filesystem_window", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Resources"))
            {
                ImGui::BeginChild("filesystem_resources", ImVec2(200, 0), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Files"))
            {
                ImGui::BeginChild("filesystem_treeview", ImVec2(200, 0), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        
        ImGui::EndChild();
        
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        ImGui::BeginChild("render_panel", ImVec2(windowWidth - 400 - (ImGui::GetStyle().ItemSpacing.x * 4), 0), false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        
        // Render panel
        if (ImGui::BeginTabBar("render_window", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Render"))
            {
                ImGui::BeginChild("render_game", ImVec2(windowWidth - 400 - (ImGui::GetStyle().ItemSpacing.x * 4), 0), false, ImGuiWindowFlags_NoDecoration);
                
                // Game view
                ImVec2 window_pos = ImGui::GetWindowPos();
                ImVec2 window_size = ImGui::GetWindowSize();
                
                if(lastWindowSize.x != window_size.x || lastWindowSize.y != window_size.y) {
                    lastWindowSize = window_size;
                    int ycoord = framebufferHeight - (window_size.y * retina) - (window_pos.y * retina); // Invert Y (from bottom)
                    renderPipeline->Initialize(window_pos.x * retina, ycoord, window_size.x * retina, window_size.y * retina);
                }

                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        ImGui::BeginChild("scene_panel", ImVec2(200, 0), false, ImGuiWindowFlags_NoDecoration);
        
        // Scene objects panel
        ImGui::BeginChild("scene_inspector", ImVec2(200, 0), false, ImGuiWindowFlags_NoDecoration);
        
        if (ImGui::BeginTabBar("inspector_window", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Inspector"))
            {
                ImGui::BeginChild("inspector_props", ImVec2(200, 0), false, ImGuiWindowFlags_NoDecoration);
                
                // Properties

                ImGui::EndChild();
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        
        ImGui::EndChild();
        
        ImGui::EndChild();
        
        ImGui::End();
        
        ImGui::ShowDemoWindow();
        
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
