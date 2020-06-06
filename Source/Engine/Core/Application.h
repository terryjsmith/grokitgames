
#ifndef application_h
#define application_h

#include <Core/System.h>
#include <Core/MetaSystem.h>

class Window;

/**
 * Singleton application class
 */
class GIGA_API Application {
public:
    Application();
    ~Application() = default;
    
    /**
     * Registered system with update tick rate
     */
    struct RegisteredSystem {
        int tickRate;
        float accumulator;
        System* system;
    };
    
    /**
     * Register a new subsystem
     */
    template<class T>
    T* CreateSystem(int tickRate = 0) {
        T* obj = new T();
        System* sys = dynamic_cast<System*>(obj);
        GIGA_ASSERT(sys != 0, "Class must be inherited from System type.");
        
        RegisteredSystem* rs = new RegisteredSystem();
        rs->tickRate = tickRate;
        rs->system = sys;
        rs->accumulator = 0;
        m_systems.push_back(rs);
        
        // Also register with meta system
        MetaSystem* metaSystem = GetSystem<MetaSystem>();
        metaSystem->SetSingleton(sys);
        
        return(obj);
    }
    
    /**
     * Find a specific subsystem by class type
     */
    template<class T>
    T* GetSystem() {
        auto i = m_systems.begin();
        for (; i != m_systems.end(); i++) {
            T* object = dynamic_cast<T*>((*i)->system);
            if (object) {
                return(object);
            }
        }
        
        return(0);
    }

    /**
     * Get application main window
     */
    Window* GetWindow() { return m_window; }
    
    /**
     * Create main application window
     */
    template<class T>
    Window* CreateAppWindow() {
        m_window = new T;
        return(m_window);
    }
    
    /**
     * Get singleton instance
     */
    static Application* GetInstance();
    
    /**
     * Update systems
     */
    void Update(float delta);
    
    /**
     * Initialize systems
     */
    void Initialize();
    
    /**
     * Log something to output file
     */
    static void Log(int type, std::string message, std::string details = "");
    
protected:
    // Currently registered systems
    std::vector<RegisteredSystem*> m_systems;
    
    // Singleton application instance
    static Application *m_instance;
    
    // Application window
    Window* m_window;
};

template<class T> T* GetSystem() {
    return(Application::GetInstance()->GetSystem<T>());
}

#endif
