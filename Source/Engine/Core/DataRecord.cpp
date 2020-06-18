
#include <Core/DataRecord.h>

DataRecord::DataRecord() {
    m_updated = false;
    m_primaryKey = 0;
}

DataRecord::~DataRecord() {
    auto it = m_vars.begin();
    for(; it != m_vars.end(); it++) {
        delete(it->second);
    }
}

void DataRecord::Set(std::string var, Variant* val) {
    auto it = m_vars.find(var);
    if(it != m_vars.end()) {
        if(it->second != val) {
            m_updated = true;
        }
        
        delete it->second;
    }
    
    m_vars[var] = val;
}

Variant* DataRecord::Get(std::string var) {
    auto it = m_vars.find(var);
    if(it != m_vars.end()) {
        return(it->second);
    }
    return(0);
}

bool DataRecord::KeyExists(std::string var) {
    auto it = m_vars.find(var);
    if(it != m_vars.end()) {
        return(true);
    }
    return(false);
}

unsigned char* DataRecord::Serialize(int& size) {
    return(0);
}

void DataRecord::Deserialize(unsigned char* data, int& size) {
    
}

std::vector<std::string> DataRecord::GetKeys() {
    std::vector<std::string> keys;
    auto it = m_vars.begin();
    for(; it != m_vars.end(); it++) {
        keys.push_back(it->first);
    }
    
    return(keys);
}
