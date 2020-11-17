
#ifndef sqlitedataloader_h
#define sqlitedataloader_h

#include <Core/DataLoader.h>
#include <sqlite3.h>

GIGA_CLASS()
class GIGA_API SQLiteDataLoader : public DataLoader {
public:
    SQLiteDataLoader();
    ~SQLiteDataLoader();
    
    GIGA_CLASS_NAME("SQLiteDataLoader");
    
    /**
     * Open a connection
     */
    GIGA_FUNCTION() virtual void Open(std::string location);
    
    /**
     * Close connection
     */
    GIGA_FUNCTION() virtual void Close() ;
    
    /**
     * Query for records
     */
    Array<GigaObject*> GetObjects(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>());
    Array<DataRecord*> GetRecords(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>());
    
    GigaObject* GetObject(std::string table, int primaryKey);
    
    /**
     * Save records
     */
    void SaveRecords(std::string table, Array<DataRecord*> records);
    void SaveObjects(std::string table, Array<GigaObject*> records);
    
    /**
     * Truncate a table/data source
     */
    virtual void Delete(std::string table, std::map<std::string, std::string> search = std::map<std::string, std::string>());
    
protected:
    static int InternalDataCallback(void* instance, int count, char** data, char** cols);
    
    bool TableExists(std::string name);
        
protected:
    // sqlite handle
    sqlite3* m_handle;
    
    // Primary key index for current table
    std::string m_tempTableName;
    
    // Current data set
    std::map<std::string, Array<DataRecord*>> m_tempRecords;
};

#endif
