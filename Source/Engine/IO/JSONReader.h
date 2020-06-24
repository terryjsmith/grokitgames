
#ifndef jsonreader_h
#define jsonreader_h

#include <IO/Resource.h>
#include <rapidjson/document.h>

class GIGA_API JSONReader {
public:
    JSONReader() = default;
    ~JSONReader() = default;
    
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
    Array<GigaObject*> GetRecords(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>());
    
    GigaObject* GetRecord(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>());
    
    /**
     * Save records
     */
    virtual void SaveRecords(std::string table, Array<DataRecord*> records);
    
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
