
#include <IO/Window.h>
#include <Core/ErrorSystem.h>
#include <IO/LogSystem.h>
#include <Core/Application.h>
#include <Render/OpenGL/OpenGL.h>
#include <GLFW/glfw3.h>

bool Window::m_initialized = false;
Window* Window::m_instance = 0;

void Window::Create(std::string name, int width, int height, bool fullscreen, bool maximized) {
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    // If we do not have a window yet, initialize it
    if (m_initialized == false) {
        glfwInit();
        
        // Specify the minimum OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        
        if(maximized) {
            glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        }
    }
    
    // Save properties
    m_width = width;
    m_height = height;
    m_name = name;
    m_fullscreen = fullscreen;
    
    // Get our primary monitor
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode * mode = glfwGetVideoMode(primary);
    unsigned int w = mode->width;
    unsigned int h = mode->height;
    
    // Create our window
    m_window = glfwCreateWindow(fullscreen ? w : width, fullscreen ? h : height, name.c_str(), fullscreen ? primary : NULL, NULL);
    if (m_window == NULL) {
        glfwTerminate();
        errorSystem->HandleError(new Error(Error::MSG_FATAL, "Unable to create window."));
        return;
    }
    
    // Make our OpenGL context current
    glfwMakeContextCurrent((GLFWwindow*)m_window);
    
    // Get our actual framebuffer size
    glfwGetFramebufferSize((GLFWwindow*)m_window, &m_framebufferWidth, &m_framebufferHeight);
    
    // Get actual window size
    glfwGetWindowSize((GLFWwindow*)m_window, &m_width, &m_height);
    
    LogSystem* logSystem = GetSystem<LogSystem>();
    logSystem->Log(Error::MSG_INFO, "Window created @ " + std::to_string(width) + " x " + std::to_string(height));
}

bool Window::IsClosing() {
    return(glfwWindowShouldClose((GLFWwindow*)m_window));
}

void Window::ProcessEvents() {
    glfwPollEvents();
}

void Window::SwapBuffer() {
    glfwSwapBuffers((GLFWwindow*)m_window);
}

Window* Window::GetInstance() {
    if(m_instance == 0) {
        m_instance = new Window();
    }
    return(m_instance);
}

void Window::GetWindowDimensions(int& width, int& height) {
    glfwGetWindowSize((GLFWwindow*)m_window, &m_width, &m_height);
    
    width = m_width;
    height = m_height;
}

void Window::GetFramebufferDimensions(int& width, int& height) {
    // Get our actual framebuffer size
    glfwGetFramebufferSize((GLFWwindow*)m_window, &m_framebufferWidth, &m_framebufferHeight);
    
    width = m_framebufferWidth;
    height = m_framebufferHeight;
}
