
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
    auto i = m_systems.begin();
    
    for (; i != m_systems.end(); i++) {
        if ((*i)->tickRate > 0) {
            (*i)->accumulator += delta;
            
            if ((*i)->accumulator > (1.0f / (*i)->tickRate)) {
                float theta = (1.0f / (*i)->tickRate);
                (*i)->accumulator -= theta;
                // (*i)->system->Update(theta);
                
                Variant* args[1];
                args[0] = new Variant(theta);
                (*i)->system->Call("Update", 1, args);
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

AppService* Application::GetAppService(std::string className) {
    auto it = m_services.begin();
    for(; it != m_services.end(); it++) {
        if((*it)->GetGigaName() == className) {
            return(*it);
        }
    }
    
    return(0);
}

void Application::RegisterAppService(AppService *service) {
    m_services.push_back(service);
}
