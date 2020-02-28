
#include <MetaClass.h>

MetaClass::MetaClass() {
    typeID = 0;
    hasPublicConstructor = false;
}

MetaClass::~MetaClass() {

}

MetaFunction* MetaClass::FindFunction(std::string name, int offset) {
    auto it = m_functions.begin();
    int counter = 0;
    for(; it != m_functions.end(); it++) {
        if((*it)->name == name) {
            if(offset == counter) {
                return((*it));
            }
            counter++;
        }
    }
    
    return(0);
}

void MetaClass::AddFunction(MetaFunction* fn) {
    m_functions.push_back(fn);
}

GigaObject* MetaClass::CreateObject() {
    return(m_ctor());
}

void MetaClass::Clear() {
    m_functions.clear();
}
