
#include <IO/JSONDataLoader.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>

void JSONDataLoader::Open(std::string location) {
    // Load resource
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    Resource* jsonFile = new Resource();
    
    std::string fullpath = resourceSystem->FindResourcePath(location);
    jsonFile->Initialize(fullpath, Resource::FILEMODE_READ);
    
    // Read data and discard
    std::string jsonData = jsonFile->GetString();
    delete jsonFile;
    
    // Parse
    m_json.Parse<rapidjson::kParseStopWhenDoneFlag>(jsonData.c_str());
}

void JSONDataLoader::Close() {
    m_json.Clear();
}

std::vector<DataRecord*> JSONDataLoader::GetRecords(std::string table, std::map<std::string, std::string> search) {
    // Initialize list of records
    std::vector<DataRecord*> records;
    
    // Get values
    bool exists = m_json.HasMember(table.c_str());
    if(m_json[table.c_str()].IsObject()) {
        DataRecord* record = new DataRecord();
        
        auto val = m_json[table.c_str()].GetObject();
        auto it = val.begin();
        for(; it != val.end(); it++) {
            record->Set((*it).name.GetString(), this->GetValue((*it).value));
        }
        
        records.push_back(record);
    }
    
    if(m_json[table.c_str()].IsArray()) {
        auto objects = m_json[table.c_str()].GetArray();
        auto it = objects.begin();
        for(; it != objects.end(); it++) {
            DataRecord* record = new DataRecord();
            
            auto obj = (*it).GetObject();
            auto props = obj.begin();
            for(; props != obj.end(); props++) {
                record->Set((*props).name.GetString(), this->GetValue((*props).value));
            }
            
            records.push_back(record);
        }
    }
    
    return(records);
}

void JSONDataLoader::SaveRecords(std::string table, std::vector<DataRecord*> records) {
    
}

void JSONDataLoader::Delete(std::string table, std::map<std::string, std::string> search) {
    
}

Variant* JSONDataLoader::GetValue(rapidjson::Value& val) {
    if(val.IsInt()) {
        return(new Variant(val.GetInt()));
    }
    
    if(val.IsUint()) {
        return(new Variant(val.GetUint()));
    }
    
    if(val.IsInt64()) {
        return(new Variant(val.GetInt64()));
    }
    
    if(val.IsUint64()) {
        return(new Variant(val.GetUint64()));
    }
    
    if(val.IsBool()) {
        return(new Variant(val.GetBool()));
    }
    
    if(val.IsFloat()) {
        return(new Variant(val.GetFloat()));
    }
    
    if(val.IsString()) {
        return(new Variant(std::string(val.GetString())));
    }
    
    if(val.IsDouble()) {
        return(new Variant((float)val.GetDouble()));
    }
    
    if(val.IsArray()) {
        Variant* var = new Variant();
        auto it = val.Begin();
        for(; it != val.End(); it++) {
            Variant* v = this->GetValue(*it);
            var->Add(v);
        }
        
        return(var);
    }
    
    return(new Variant(0));
}
