
#include <Render/GuiSystem.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <IO/Window.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

void GuiSystem::Startup() {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Window* window = Window::GetInstance();
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
    GLFWwindow* glfwWindow = (GLFWwindow*)window->GetWindowHandle();
    char* glsl_version = "#version 400";

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void GuiSystem::StartFrame() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiSystem::EndFrame() {
    
}

void GuiSystem::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiSystem::Shutdown() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
