
#ifndef sqlitedataloader_h
#define sqlitedataloader_h

#include <IO/DataLoader.h>
#include <sqlite3.h>

class GIGA_API SQLiteDataLoader : public DataLoader {
public:
    SQLiteDataLoader();
    ~SQLiteDataLoader();
    
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
    static int InternalDataCallback(void* instance, int count, char** data, char** cols);
    
    bool TableExists(std::string name);
        
protected:
    // sqlite handle
    sqlite3* m_handle;
    
    // Primary key index for current table
    std::string m_tempPrimaryKey;
    
    // Current data set
    std::vector<DataRecord*> m_tempRecords;
};

#endif
