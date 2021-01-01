
#ifndef window_h
#define window_h

#include <Core/GigaObject.h>

class GIGA_API Window : public GigaObject {
public:
    Window() = default;
    ~Window() = default;
    
    GIGA_CLASS_NAME("Window");
    
    /**
     * Create a new window
     */
    void Create(std::string name, int width, int height, bool fullscreen, bool maximized = false);
    
    /**
     * Whether the window is closed/closing or not
     */
    bool IsClosing();
    
    /**
     * Platform-specific event loop
     */
    void ProcessEvents();
    
    /**
     * Swap window buffers
     */
    void SwapBuffer();
    
    /**
     * Getters
     */
    void GetWindowDimensions(int& width, int& height);
    void GetFramebufferDimensions(int& width, int& height);
    
    std::string GetName() { return m_name; }
    bool IsFullscreen() { return m_fullscreen; }
    
    void* GetWindowHandle() { return m_window; }
    
    /**
     * Singleton
     */
    static Window* GetInstance();
    
protected:
    // Window width and height
    int m_width, m_height;
    
    bool m_fullscreen;
    std::string m_name;
    
    // Frame width and height (different for retina)
    int m_framebufferWidth, m_framebufferHeight;
    
    // Whether GLFW and other window libraries are initialized yet
    static bool m_initialized;
    
    // Internal window pointer
    void *m_window;
    
    // Singleton
    static Window* m_instance;
};

#endif
