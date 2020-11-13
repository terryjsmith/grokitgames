
#ifndef mysqldataloader_h
#define mysqldataloader_h

#include <mysql.h>
#include <Core/DataLoader.h>

class MySQLDataLoader : public DataLoader {
public:
    MySQLDataLoader();
    ~MySQLDataLoader();
    
    /**
     * Connect to a MySQL server
     */
    bool Connect(std::string url, std::string username, std::string password, std::string db);
    
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
     * Create or alter any necessary tables for registered types
     */
    void UpdateTables();
    
protected:
    std::string m_url, m_username, m_password, m_db;
    MYSQL *m_connection;
};

#endif
