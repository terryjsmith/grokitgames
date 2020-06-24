
#include <Core/GigaObject.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>

Array<GigaObject*> GigaObject::m_objects = Array<GigaObject*>();

GigaObject::GigaObject() {
    m_mutex = 0;
    m_objects.push_back(this);
}

GigaObject::~GigaObject() {
    auto it = m_objects.find(this);
    if(it != m_objects.end()) {
        m_objects.erase(it);
    }
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

Array<GigaObject*> GigaObject::GetObjects(std::string className) {
    Array<GigaObject*> vec;
    
    auto it = m_objects.begin();
    for(; it != m_objects.end(); it++) {
        GigaObject* obj = dynamic_cast<GigaObject*>(*it);
        if(obj) {
            if(obj->GetGigaName() == className) {
                vec.push_back(*it);
            }
        }
    }
    
    return(vec);
}

std::string GigaObject::ToString() {
    return(this->GetGigaName());
}
