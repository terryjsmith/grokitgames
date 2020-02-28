
#include <Core/GigaObject.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>

Meta::Class* GigaObject::GetClass() {
    if(m_mc == 0) {
        MetaSystem* ms = GetSystem<MetaSystem>();
        m_mc = ms->FindClass(this->GetGigaName());
    }
    
    return(m_mc);
}
