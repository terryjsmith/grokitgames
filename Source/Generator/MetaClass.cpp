
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

bool MetaClass::AddFunction(MetaFunction* fn, bool checkSignature) {
    if(checkSignature) {
        // Attempt to find a function with a matching signature first
        bool match = false;
        int counter = 0;
        MetaFunction* mf = FindFunction(fn->name, counter);
        while(mf != 0) {
            if(fn->params.size() == mf->params.size()) {
                if(fn->returnType == mf->returnType) {
                    auto it = fn->params.begin();
                    bool pmatch = true;
                    for(; it != fn->params.end(); it++) {
                        auto mfit = mf->params.begin();
                        bool found = false;
                        for(; mfit != mf->params.end(); mfit++) {
                            if((*mfit)->name.compare((*it)->name) == 0 && (*mfit)->type == (*it)->type)
                                found = true;
                        }
                               
                        if(found == false) {
                            pmatch = false;
                            break;
                        }
                    }
                    
                    if(pmatch == true) {
                        match = true;
                        return(false); // Matching signature
                    }
                }
            }
            
            counter++;
            mf = FindFunction(fn->name, counter);
        }
    }
    
    m_functions.push_back(fn);
    return(true);
}

bool MetaClass::AddVariable(MetaVariable* prop, bool checkSignature) {
    if(checkSignature) {
        auto it = m_variables.begin();
        for(; it != m_variables.end(); it++) {
            if((*it)->name.compare(prop->name) == 0) {
                return(false);
            }
        }
    }
    
    m_variables.push_back(prop);
    return(true);
}

MetaVariable* MetaClass::FindVariable(std::string name) {
    auto it = m_variables.begin();
    for(; it != m_variables.end(); it++) {
        if((*it)->name.compare(name) == 0) {
            return(*it);
        }
    }
    
    return(0);
}

GigaObject* MetaClass::CreateObject() {
    return(m_ctor());
}

void MetaClass::Clear() {
    m_functions.clear();
}
