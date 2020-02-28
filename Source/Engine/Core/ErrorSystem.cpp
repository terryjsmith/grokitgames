
#include <Core/ErrorSystem.h>
#include <Core/MetaSystem.h>
#include <Core/TimeSystem.h>
#include <Core/Application.h>

void ErrorSystem::Initialize() {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    metaSystem->SetSingleton(this);
}

void ErrorSystem::HandleError(Error* err) {
    auto it = m_handlers.begin();
    for(; it != m_handlers.end(); it++) {
        if((*it)->level == err->level) {
            (*it)->func(err);
        }
    }
}

void ErrorSystem::RegisterErrorHandler(int level, ErrorHandlingFunction func) {
    ErrorHandler* handler = new ErrorHandler();
    handler->level = level;
    handler->func = func;
    
    m_handlers.push_back(handler);
}
