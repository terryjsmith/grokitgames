
#include <Core/Meta.h>

namespace Meta {

Meta::Class::Class() {
    typeID = 0;
    hasPublicConstructor = false;
}

Meta::Class::~Class() {

}

Meta::Function* Meta::Class::FindFunction(std::string name) {
    auto it = m_functions.begin();
    for(; it != m_functions.end(); it++) {
        if((*it)->name == name) {
            return((*it));
        }
    }
    
    return(0);
}

void Meta::Class::AddFunction(Meta::Function* fn) {
    m_functions.push_back(fn);
}

void Meta::Class::AddVariable(Meta::Variable* fn) {
    m_variables.push_back(fn);
}

bool Meta::Class::InheritsFrom(std::string className) {
    return(inheritsFrom.find(className) == inheritsFrom.end());
}

Meta::Variable* Meta::Class::FindVariable(std::string name) {
    auto it = m_variables.begin();
    for(; it != m_variables.end(); it++) {
        if((*it)->name == name) {
            return((*it));
        }
    }
    
    return(0);
}

GigaObject* Meta::Class::CreateObject() {
    return(m_ctor());
}

}
