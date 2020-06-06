
#include <IO/SQLiteDataLoader.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>

SQLiteDataLoader::SQLiteDataLoader() {
    m_handle = 0;
}

SQLiteDataLoader::~SQLiteDataLoader() {
    if(m_handle) {
        sqlite3_close(m_handle);
    }
}

void SQLiteDataLoader::Open(std::string location) {
    int result = sqlite3_open(location.c_str(), &m_handle);
    if(result != 0) {
        ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to ope SQLite file.", location));
        sqlite3_close(m_handle);
        m_handle = 0;
    }
}

void SQLiteDataLoader::Close() {
    if(m_handle) {
        sqlite3_close(m_handle);
        m_handle = 0;
    }
}

std::vector<DataRecord*> SQLiteDataLoader::GetRecords(std::string table, std::map<std::string, std::string> search) {
    // Clear any previous temporary records
    m_tempRecords.clear();
    
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    if(this->TableExists(table) == false) {
        return(m_tempRecords);
    }
    
    // Get table structure
    std::string query = "PRAGMA table_info(" + table + ")";
    if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get SQLite schema.", (char*)sqlite3_errmsg(m_handle)));
        GIGA_ASSERT(false, "Unable to get SQLite schema.");
        return(m_tempRecords);
    }
    
    // Find the primary key
    sqlite3_stmt* stmt = 0;
    int result = 0;
    uint32_t currentIndex = 0;
    result = sqlite3_prepare(m_handle, query.c_str(), (int)query.length() + 1, &stmt, 0);
    while((result = sqlite3_step(stmt))) {
        if(result != SQLITE_ROW) {
            break;
        }
        
        std::string colname = (const char*)sqlite3_column_text(stmt, 1);
        int pk = sqlite3_column_int(stmt, 5);
        
        if(pk == 1) {
            m_tempPrimaryKey = colname;
        }
        
        currentIndex++;
    }
    
    query = "SELECT * FROM " + table;
    
    // Add where clauses
    if (search.size()) {
        query += " WHERE 1=1 ";
        std::map<std::string, std::string>::iterator it = search.begin();
        for (; it != search.end(); it++) {
            query += " AND " + it->first + " = '" + it->second + "'";
        }
    }
    
    // Execute
    if(sqlite3_exec(m_handle, query.c_str(), InternalDataCallback, this, 0) != 0) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get record list from SQLite.", (char*)sqlite3_errmsg(m_handle)));
        GIGA_ASSERT(false, "Unable to get record list from SQLite.");
        return(m_tempRecords);
    }
    
    return(m_tempRecords);
}

void SQLiteDataLoader::SaveRecords(std::string table, std::vector<DataRecord*> records) {
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    // If nothing to save, exit
    if(records.size() == 0) {
        return;
    }
    
    // We will use the first record to determine structure
    DataRecord* firstRecord = records[0];
    
    // Create a table if one does not exist
    std::string query = "CREATE TABLE IF NOT EXISTS ";
    query += table + " (";
    
    query += table + "_id INTEGER";
    
    std::vector<std::string> fields = firstRecord->GetKeys();
    auto fi = fields.begin();
    for(; fi != fields.end(); fi++) {
        query += "," + (*fi) + " TEXT";
    }
    
    query += ", PRIMARY KEY(" + table + "_id ASC))";
    
    if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to create SQLite table.", (char*)sqlite3_errmsg(m_handle)));
        GIGA_ASSERT(false, "Unable to create SQLite table.");
        return;
    }
    
    // Add any columns that may not yet exist
    query = "PRAGMA table_info(" + table + ")";
    if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get SQLite schema.", (char*)sqlite3_errmsg(m_handle)));
        GIGA_ASSERT(false, "Unable to get SQLite schema.");
        return;
    }
    
    // Put the column names into a vector
    sqlite3_stmt* stmt = 0;
    int result = 0;
    std::vector<std::string> existingCols;
    result = sqlite3_prepare(m_handle, query.c_str(), (int)query.length() + 1, &stmt, 0);
    while((result = sqlite3_step(stmt))) {
        if(result != SQLITE_ROW) {
            break;
        }
        
        std::string colname = (const char*)sqlite3_column_text(stmt, 1);
        existingCols.push_back(colname);
        
        int pk = sqlite3_column_int(stmt, 5);
        if(pk == 1) {
            m_tempPrimaryKey = colname;
        }
    }
    
    fi = fields.begin();
    for(; fi != fields.end(); fi++) {
        auto it = std::find(existingCols.begin(), existingCols.end(), (*fi));
        if(it == existingCols.end()) {
            query = "ALTER TABLE " + table + " ADD COLUMN " + (*fi) + " TEXT";
            
            if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to alter SQLite table.",     (char*)sqlite3_errmsg(m_handle)));
                GIGA_ASSERT(false, "Unable to alter SQLite DB.");
                return;
            }
        }
    }
    
    // Save records
    auto ri = records.begin();
    for(; ri != records.end(); ri++) {
        // Process new record
        if((*ri)->GetID() == 0) {
            // Insert
            query = "INSERT INTO " + table + " (";
            std::string values = ") VALUES (";
            auto f = fields.begin();
            query += (*f);
            values += "'" + (*ri)->Get(*f)->ToString() + "'";
            f++;
                
            for(; f != fields.end(); f++) {
                std::string field = (*f);
                if(field.compare(m_tempPrimaryKey) != 0) {
                    query += "," + field;
                    values += ",'" + (*ri)->Get(field)->ToString() + "'";
                }
            }
                
            query += values + ")";
                
            if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to insert object from SQLite.", (char*)sqlite3_errmsg(m_handle)));
                GIGA_ASSERT(false, "Unable to insert object in SQLite DB.");
            }
                
            unsigned int newID = (unsigned int)sqlite3_last_insert_rowid(m_handle);
            if(newID == 0) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to insert object from SQLite.", (char*)sqlite3_errmsg(m_handle)));
                GIGA_ASSERT(false, "Unable to insert object in SQLite DB.");
            }
                
            (*ri)->SetID(newID);
            continue;
        }
        
        // Get the primary key
        char primaryKeyID[10];
        sprintf(primaryKeyID, "%d", (*ri)->GetID());
        
        // Process deletion
        if((*ri)->IsDeleted()) {
            // Construct query
            query = "DELETE FROM " + table + " WHERE " + m_tempPrimaryKey+ " = ";
            query += primaryKeyID;
                
            if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to delete object from SQLite.", (char*)sqlite3_errmsg(m_handle)));
                GIGA_ASSERT(false, "Unable to delete object from SQLite.");
            }

            continue;
        }
        
        // Updated record
        query = "UPDATE " + table + " SET ";
        auto f = fields.begin();
        query += (*f) + " = '" + (*ri)->Get(*f)->ToString() + "'";
        f++;
            
        for(; f != fields.end(); f++) {
            std::string field = (*f);
            if(field.compare(m_tempPrimaryKey) != 0) {
                query += "," + field + " = '" + (*ri)->Get(field)->ToString() + "'";
            }
        }
            
        query += " WHERE " + m_tempPrimaryKey + " = " + primaryKeyID;
            
        if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
            errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to update object from SQLite.", (char*)sqlite3_errmsg(m_handle)));
            GIGA_ASSERT(false, "Unable to update object in SQLite DB.");
        }
    }
}

int SQLiteDataLoader::InternalDataCallback(void* instance, int count, char** data, char** cols) {
    DataRecord* record = new DataRecord();
    SQLiteDataLoader* loader = (SQLiteDataLoader*)instance;
    
    // Iterate over count columns of data
    for(int i = 0; i < count; i++) {
        if(data[i] == 0) {
            continue;
        }
        
        if(loader->m_tempPrimaryKey.compare(cols[i]) == 0) {
            record->SetID(atoi(data[i]));
        }
    
        record->Set(cols[i], new Variant(data[i]));
    }
    
    loader->m_tempRecords.push_back(record);
    
    return(0);
}

void SQLiteDataLoader::Delete(std::string table, std::map<std::string, std::string> search) {
    if(this->TableExists(table) == false) {
        return;
    }
    
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    std::string query = "DELETE FROM " + table;
    
    // Add where clauses
    if (search.size()) {
        query += " WHERE 1=1 ";
        std::map<std::string, std::string>::iterator it = search.begin();
        for (; it != search.end(); it++) {
            query += " AND " + it->first + " = '" + it->second + "'";
        }
    }
    
    // Execute
    if(sqlite3_exec(m_handle, query.c_str(), InternalDataCallback, this, 0) != 0) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to delete from SQLite.", (char*)sqlite3_errmsg(m_handle)));
        GIGA_ASSERT(false, "Unable to delete from SQLite.");
    }
}

bool SQLiteDataLoader::TableExists(std::string name) {
    // Check if the table exists
    sqlite3_stmt* stmt = 0;
    int result = 0;
    bool tableExists = false;
    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + name + "';";
    result = sqlite3_prepare(m_handle, query.c_str(), (int)query.length() + 1, &stmt, 0);
    while((result = sqlite3_step(stmt))) {
        if(result != SQLITE_ROW) {
            break;
        }
        
        tableExists = true;
    }
    
    sqlite3_finalize(stmt);
    return(tableExists);
}
