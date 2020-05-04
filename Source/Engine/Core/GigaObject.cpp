
#include <Core/GigaObject.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>

GigaObject::GigaObject() {
    m_mutex = 0;
}

Meta::Class* GigaObject::GetClass() {
    if(m_mc == 0) {
        MetaSystem* ms = GetSystem<MetaSystem>();
        m_mc = ms->FindClass(this->GetGigaName());
    }
    
    return(m_mc);
}

Variant* GigaObject::Call(std::string func, int argc, Variant** argv) {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Meta::Class* cl = metaSystem->FindClass(this->GetGigaName());
    GIGA_ASSERT(cl != 0, "Class not found.");
    
    Meta::Function* fn = cl->FindFunction(func);
    GIGA_ASSERT(fn != 0, "Function not found.");
    
    return(fn->func(this, argc, argv));
}

void GigaObject::Lock() {
    if(m_mutex == 0) {
        m_mutex = new std::mutex;
    }
    
    m_mutex->lock();
}

void GigaObject::Unlock() {
    GIGA_ASSERT(m_mutex != 0, "Cannot unlock non-initiated mutex.");
    m_mutex->unlock();
}
