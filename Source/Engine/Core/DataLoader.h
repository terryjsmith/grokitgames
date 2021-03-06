
#ifndef dataloader_h
#define dataloader_h

#include <Core/AppService.h>
#include <Core/DataRecord.h>

GIGA_CLASS()
class GIGA_API DataLoader : public AppService {
public:
    virtual ~DataLoader() = default;
    
    /**
     * Open a connection
     */
    virtual void Open(std::string location) { }
    
    /**
     * Close connection
     */
    virtual void Close() { }
    
    /**
     * Query for records
     */
    virtual Array<GigaObject*> GetObjects(std::string table, std::map<std::string, std::string> search) = 0;
    GIGA_FUNCTION() Array<GigaObject*> GetObjects(std::string table);
    virtual GigaObject* GetObject(std::string table, int primaryKey) = 0;
    
    virtual Array<DataRecord*> GetRecords(std::string table, std::map<std::string, std::string> search) = 0;
    GIGA_FUNCTION() Array<DataRecord*> GetRecords(std::string table);
    
    /**
     * Save records
     */
    virtual void SaveRecords(std::string table, Array<DataRecord*> records) = 0;
    virtual void SaveObjects(std::string table, Array<GigaObject*> records) = 0;
    
    /**
     * Truncate a table/data source
     */
    virtual void Delete(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>()) { }
    
protected:
    // No direct creation
    DataLoader() = default;
    
protected:
    // Open connection
    std::string m_location;
    
    // Cache of data record to objects
    std::map<GigaObject*, DataRecord*> m_recordCache;
    
    // Cache of all data records loaded
    std::map<std::string, std::map<uint32_t, DataRecord*>> m_records;
    
    // Cache of primary keys to existing objects
    std::map<std::string, std::map<uint32_t, GigaObject*>> m_objectCache;
};

#endif
