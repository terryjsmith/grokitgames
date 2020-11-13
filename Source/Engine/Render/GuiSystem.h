
#ifndef guisystem_h
#define guisystem_h

#include "imgui.h"
#include <Core/System.h>

GIGA_CLASS()
class GIGA_API GuiSystem : public System {
public:
    GuiSystem() = default;
    ~GuiSystem() = default;
    
    GIGA_CLASS_NAME("GuiSystem");
    
    /**
     * Initialize ImGui
     */
    void Startup();
    
    /**
     * Start frame
     */
    void StartFrame();
    
    /**
     * End frame
     */
    void EndFrame();
    
    /**
     * Render
     */
    void Render();
    
    /**
     * Shutdown / clean up
     */
    void Shutdown();
    
};

#endif
