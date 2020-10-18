
#include <Core/DataRecord.h>
#include <IO/MemoryWriter.h>
#include <IO/MemoryReader.h>

DataRecord::DataRecord() {
    m_updated = false;
    m_primaryKey = 0;
    m_deleted = false;
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

Array<std::string> DataRecord::GetKeys() {
    Array<std::string> keys;
    
    auto it = m_vars.begin();
    for(; it != m_vars.end(); it++) {
        keys.push_back(it->first);
    }
    
    return(keys);
}
