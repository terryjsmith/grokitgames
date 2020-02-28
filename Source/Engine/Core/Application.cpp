
#include <Core/Application.h>

Application* Application::m_instance = 0;

Application::Application() {
    m_window = 0;
}

Application* Application::GetInstance() {
    if(m_instance == 0) {
        m_instance = new Application();
    }
    
    return(m_instance);
}

void Application::Initialize() {
    auto it = m_systems.begin();
    for(; it != m_systems.end(); it++) {
        (*it)->system->Initialize();
    }
}

void Application::Update(float delta) {
    std::vector<RegisteredSystem*>::iterator i = m_systems.begin();
    
    for (; i != m_systems.end(); i++) {
        if ((*i)->tickRate > 0) {
            (*i)->accumulator += delta;
            
            if ((*i)->accumulator > (1.0f / (*i)->tickRate)) {
                float theta = (1.0f / (*i)->tickRate);
                (*i)->accumulator -= theta;
                (*i)->system->Update(theta);
            }
        }
    }
    
    i = m_systems.begin();
    for (; i != m_systems.end(); i++) {
        if ((*i)->tickRate == 0) {
            (*i)->system->Update(delta);
        }
    }
}
