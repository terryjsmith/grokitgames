
#include <Core/MetaSystem.h>

void MetaSystem::Initialize() {    
    // Register yourself as a singleton
    this->SetSingleton(this);
}

Meta::Class* MetaSystem::FindClass(std::string name) {
    auto it = m_definitions.begin();
    for(; it != m_definitions.end(); it++) {
        Meta::Class* cl = (*it);
        if(cl->name == name) {
            return(cl);
        }
    }
    
    return(0);
}

Meta::Class* MetaSystem::FindClass(uint32_t typeID) {
    auto it = m_definitions.begin();
    for(; it != m_definitions.end(); it++) {
        Meta::Class* cl = (*it);
        if(cl->typeID == typeID) {
            return(cl);
        }
    }
    
    return(0);
}

GigaObject* MetaSystem::CreateClass(std::string name) {
    Meta::Class* cl = this->FindClass(name);
    GIGA_ASSERT(cl != 0, "Class name not found.");
    
    return(cl->CreateObject());
}

GigaObject* MetaSystem::CreateClass(uint32_t typeID) {
   Meta::Class* cl = this->FindClass(typeID);
    GIGA_ASSERT(cl != 0, "Class name not found.");
    
    return(cl->CreateObject());
}

void MetaSystem::AddClass(Meta::Class* cl) {
    // Check for not already registered
    Meta::Class* exists = this->FindClass(cl->typeID);
    GIGA_ASSERT(exists == 0, "Type ID already registered.");
    
    m_maxTypeID = std::max(cl->typeID, m_maxTypeID);
    
    m_definitions.push_back(cl);
}

GigaObject* MetaSystem::GetSingleton(std::string className) {
    auto it = m_singletons.begin();
    for(; it != m_singletons.end(); it++) {
        GigaObject* obj = (*it);
        if(obj->GetGigaName() == className) {
            return(obj);
        }
    }
    
    return(0);
}

void MetaSystem::SetSingleton(GigaObject* obj) {
    for(size_t i = 0; i < m_singletons.size(); i++) {
        if(m_singletons[i]->GetGigaName() == obj->GetGigaName()) {
            m_singletons[i] = obj;
            return;
        }
    }
    
    m_singletons.push_back(obj);
}
