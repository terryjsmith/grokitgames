
#include <IO/SQLiteDataLoader.h>
#include <Core/ErrorSystem.h>
#include <Core/MetaSystem.h>
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

std::vector<GigaObject*> SQLiteDataLoader::GetObjects(std::string table, std::map<std::string, std::string> search) {
    std::vector<GigaObject*> retval;
    
    // Store and restore temp table name (nested gets)
    std::string prevTableName = m_tempTableName;
    m_tempTableName = table;
    
    // Clear any previous temporary records
    auto ri = m_tempRecords.find(table);
    if(ri != m_tempRecords.end()) {
        ri->second.clear();
    }
    else {
        m_tempRecords[table] = std::vector<DataRecord*>();
    }
    
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    if(this->TableExists(table) == false) {
        m_tempTableName = prevTableName;
        return(retval);
    }
    
    // Get table structure
    std::string query = "PRAGMA table_info(" + table + ")";
    if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get SQLite schema.", (char*)sqlite3_errmsg(m_handle)));
        GIGA_ASSERT(false, "Unable to get SQLite schema.");
        m_tempTableName = prevTableName;
        return(retval);
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
        m_tempTableName = prevTableName;
        return(retval);
    }
    
    // Process objects
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    
    std::string primaryKeyCol = table + "_id";
    auto ci = m_objectCache.find(table);
    if(ci == m_objectCache.end()) {
        m_objectCache[table] = std::map<uint32_t, GigaObject*>();
        ci = m_objectCache.find(table);
    }
    
    auto oi = m_tempRecords[table].begin();
    for(; oi != m_tempRecords[table].end(); oi++) {
        // Check cache for existing object
        GigaObject* obj = 0;
        uint32_t primaryKeyValue = (*oi)->GetID();
        auto obi = ci->second.find(primaryKeyValue);
        if(obi != ci->second.end()) {
            obj = obi->second;
        }
        else {
            obj = metaSystem->CreateClass(table);
            ci->second[primaryKeyValue] = obj;
        }
        
        // Deserialize data record
        obj->Deserialize(*oi);
        
        m_recordCache[obj] = *oi;
        retval.push_back(obj);
    }
    
    m_tempTableName = prevTableName;
    return(retval);
}


std::vector<DataRecord*> SQLiteDataLoader::GetRecords(std::string table, std::map<std::string, std::string> search) {
    // Store and restore temp table name (nested gets)
    std::string prevTableName = m_tempTableName;
    m_tempTableName = table;
    
    // Clear any previous temporary records
    auto ri = m_tempRecords.find(table);
    if(ri != m_tempRecords.end()) {
        ri->second.clear();
    }
    else {
        m_tempRecords[table] = std::vector<DataRecord*>();
    }
    
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    if(this->TableExists(table) == false) {
        m_tempTableName = prevTableName;
        return(m_tempRecords[table]);
    }
    
    // Get table structure
    std::string query = "PRAGMA table_info(" + table + ")";
    if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get SQLite schema.", (char*)sqlite3_errmsg(m_handle)));
        GIGA_ASSERT(false, "Unable to get SQLite schema.");
        m_tempTableName = prevTableName;
        return(m_tempRecords[table]);
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
        m_tempTableName = prevTableName;
        return(m_tempRecords[table]);
    }
    
    m_tempTableName = prevTableName;
    return(m_tempRecords[table]);
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
    std::string primaryKeyCol = table + "_id";
    auto ri = records.begin();
    for(; ri != records.end(); ri++) {
        DataRecord* dr = *ri;
        
        // Process new record
        if(dr->GetID() == 0) {
            // Insert
            query = "INSERT INTO " + table + " (";
            std::string values = ") VALUES (";
            auto f = fields.begin();
            for(; f != fields.end(); f++) {
                std::string field = (*f);
                if(field.compare(primaryKeyCol) != 0) {
                    Variant* v = dr->Get(field);
                    std::string value = std::to_string(v->GetType()) + ":" + v->ToString();
                    
                    if(f != fields.begin()) {
                        query += ",";
                        values += ",";
                    }
                    query += field;
                    values += "'" + value + "'";
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
                
            dr->SetID(newID);
            continue;
        }
        
        // Get the primary key
        char primaryKeyID[10];
        sprintf(primaryKeyID, "%d", dr->GetID());
        
        // Process deletion
        if(dr->IsDeleted()) {
            // Construct query
            query = "DELETE FROM " + table + " WHERE " + primaryKeyCol + " = ";
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
        for(; f != fields.end(); f++) {
            std::string field = (*f);
            if(field.compare(primaryKeyCol) != 0) {
                Variant* v = dr->Get(*f);
                std::string value = std::to_string(v->GetType()) + ":" + dr->Get(field)->ToString();
                
                if(f != fields.begin()) query += ",";
                query += field + " = '" + value + "'";
            }
        }

        query += " WHERE " + primaryKeyCol  + " = " + primaryKeyID;
            
        if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
            errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to update object from SQLite.", (char*)sqlite3_errmsg(m_handle)));
            GIGA_ASSERT(false, "Unable to update object in SQLite DB.");
        }
    }
}

void SQLiteDataLoader::SaveObjects(std::string table, std::vector<GigaObject*> records) {
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    // If nothing to save, exit
    if(records.size() == 0) {
        return;
    }
    
    // We will use the first record to determine structure
    DataRecord* firstRecord = 0;
    auto di = m_recordCache.find(records[0]);
    if(di == m_recordCache.end()) {
        firstRecord = new DataRecord();
        records[0]->Serialize(firstRecord);
        m_recordCache[records[0]] = firstRecord;
    }
    else {
        firstRecord = di->second;
    }
    
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
    std::string primaryKeyCol = table + "_id";
    auto ri = records.begin();
    for(; ri != records.end(); ri++) {
        // Find an associated data record, or create one
        DataRecord* dr = 0;
        auto di = m_recordCache.find(*ri);
        if(di == m_recordCache.end()) {
            dr = new DataRecord();
            m_recordCache[*ri] = dr;
        }
        else {
            dr = di->second;
        }

        (*ri)->Serialize(dr);
        
        // Process new record
        if(dr->GetID() == 0) {
            // Insert
            query = "INSERT INTO " + table + " (";
            std::string values = ") VALUES (";
            auto f = fields.begin();
            for(; f != fields.end(); f++) {
                std::string field = (*f);
                if(field.compare(primaryKeyCol) != 0) {
                    Variant* v = dr->Get(field);
                    std::string value = std::to_string(v->GetType()) + ":" + dr->Get(field)->ToString();
                    if(v->IsObject()) {
                        if(v->AsObject() == 0) {
                            DataRecord::TypeHint* hint = dr->GetTypeHint(*f);
                            std::string value = std::to_string(v->GetType()) + ":" + hint->type + ":0";
                        }
                        else {
                            // Look for a cached object
                            auto oi = m_recordCache.find(v->AsObject());
                            if(oi != m_recordCache.end()) {
                                // Update the cached object ID
                                value = std::to_string(v->GetType()) + ":" + oi->first->GetGigaName() + ":" + std::to_string(oi->second->GetID());
                            }
                            else {
                                // Create a new object, save ID
                                GigaObject* obj = v->AsObject();
                                std::vector<GigaObject*> newRecord;
                                newRecord.push_back(obj);
                                
                                this->SaveObjects(obj->GetGigaName(), newRecord);
                                
                                // Get the new object from the cache
                                DataRecord* newDR = m_recordCache[obj];
                                
                                // Get ID
                                value = std::to_string(v->GetType()) + ":" + obj->GetGigaName() + ":" + std::to_string(newDR->GetID());
                            }
                        }
                    }
                    
                    if(f != fields.begin()) {
                        query += ",";
                        values += ",";
                    }
                    query += field;
                    values += "'" + value + "'";
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
                
            dr->SetID(newID);
            continue;
        }
        
        // Get the primary key
        char primaryKeyID[10];
        sprintf(primaryKeyID, "%d", dr->GetID());
        
        // Process deletion
        if(dr->IsDeleted()) {
            // Construct query
            query = "DELETE FROM " + table + " WHERE " + primaryKeyCol + " = ";
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
        for(; f != fields.end(); f++) {
            std::string field = (*f);
            if(field.compare(primaryKeyCol) != 0) {
                Variant* v = dr->Get(field);
                std::string value = std::to_string(v->GetType()) + ":" + dr->Get(field)->ToString();
                if(v->IsObject()) {
                    if(v->AsObject() == 0) {
                        DataRecord::TypeHint* hint = dr->GetTypeHint(*f);
                        std::string value = std::to_string(v->GetType()) + ":" + hint->type + ":0";
                    }
                    else {
                        // Save / update
                        GigaObject* obj = v->AsObject();
                        std::vector<GigaObject*> newRecord;
                        newRecord.push_back(obj);

                        // Save
                        this->SaveObjects(obj->GetGigaName(), newRecord);

                        // Look for a cached object
                        auto oi = m_recordCache.find(v->AsObject());
                        value = std::to_string(v->GetType()) + ":" + oi->first->GetGigaName() + ":" + std::to_string(oi->second->GetID());
                    }
                }
                
                if(f != fields.begin()) query += ",";
                query += field + " = '" + value + "'";
            }
        }
            
        query += " WHERE " + primaryKeyCol + " = " + primaryKeyID;
            
        if(sqlite3_exec(m_handle, query.c_str(), 0, 0, 0) != 0) {
            errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to update object from SQLite.", (char*)sqlite3_errmsg(m_handle)));
            GIGA_ASSERT(false, "Unable to update object in SQLite DB.");
        }
    }
}

GigaObject* SQLiteDataLoader::GetObject(std::string table, int primaryKey) {
    // First check our cache
    auto ci = m_objectCache.find(table);
    if(ci != m_objectCache.end()) {
        auto oi = m_objectCache[table].find(primaryKey);
        if(oi != m_objectCache[table].end()) return(oi->second);
    }
    
    std::map<std::string, std::string> search;
    search[table + "_id"] = std::to_string(primaryKey);
    
    std::vector<GigaObject*> records = this->GetObjects(table, search);
    if(records.size()) {
        return(records[0]);
    }
    
    return(0);
}

int SQLiteDataLoader::InternalDataCallback(void* instance, int count, char** data, char** cols) {
    DataRecord* record = new DataRecord();
    SQLiteDataLoader* loader = (SQLiteDataLoader*)instance;
    
    auto ci = loader->m_records.find(loader->m_tempTableName);
    if(ci == loader->m_records.end()) {
        loader->m_records[loader->m_tempTableName] = std::map<uint32_t, DataRecord*>();
        ci = loader->m_records.find(loader->m_tempTableName);
    }
    
    // Iterate over count columns of data
    std::string primaryKeyCol = loader->m_tempTableName + "_id";
    uint32_t primaryKeyVal = 0;
    for(int i = 0; i < count; i++) {
        if(data[i] == 0) {
            continue;
        }
        
        if(primaryKeyCol.compare(cols[i]) == 0) {
            // Check our cache for an existing record to update
            primaryKeyVal = atoi(data[i]);
            auto cache = ci->second.find(primaryKeyVal);
            if(cache != ci->second.end()) {
                delete record;
                record = cache->second;
            }
            else {
                ci->second[primaryKeyVal] = record;
            }
            
            record->SetID(primaryKeyVal);
            continue;
        }
    
        // Read type and value
        std::string fieldValue = data[i];
        int type = atoi(fieldValue.substr(0, fieldValue.find(":")).c_str());
        std::string value = fieldValue.substr(fieldValue.find(":") + 1);
        
        Variant* v = 0;
        
        if(type == Variant::VAR_OBJECT) {
            std::string lookup = value.substr(value.find(":") + 1);
            std::string className = value.substr(0, value.find(":"));
            
            GigaObject* obj = 0;
            if(lookup != "0") {
                obj = loader->GetObject(className, atoi(lookup.c_str()));
            }
            v = new Variant(obj);
        }
        else {
            v = new Variant();
            v->FromString(value, type);
        }
        
        record->Set(cols[i], v);
    }
    
    loader->m_tempRecords[loader->m_tempTableName].push_back(record);
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
