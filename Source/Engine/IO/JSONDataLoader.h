
#ifndef jsondataloader_h
#define jsondataloader_h

#include <IO/DataLoader.h>
#include <IO/Resource.h>
#include <rapidjson/document.h>

class GIGA_API JSONDataLoader : public DataLoader {
public:
    JSONDataLoader() = default;
    ~JSONDataLoader() = default;
    
    /**
     * Open a connection
     */
    virtual void Open(std::string location);
    
    /**
     * Close connection
     */
    virtual void Close() ;
    
    /**
     * Query for records
     */
    virtual std::vector<DataRecord*> GetRecords(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>());
    
    /**
     * Save records
     */
    virtual void SaveRecords(std::string table, std::vector<DataRecord*> records);
    
    /**
     * Truncate a table/data source
     */
    virtual void Delete(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>());
    
protected:
    Variant* GetValue(rapidjson::Value& val);
    
protected:
    rapidjson::Document m_json;
};

#endif
