
#include <IO/MySQLDataLoader.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>

MySQLDataLoader::MySQLDataLoader() {
    m_handle = 0;
}

MySQLDataLoader::~MySQLDataLoader() {
    if (m_handle) {
        mysql_close(m_handle);
    }
}

bool MySQLDataLoader::Connect(std::string host, int port, std::string username, std::string password, std::string db) {
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    m_handle = mysql_init(NULL);
    
    m_db = db;

    if (mysql_real_connect(m_handle, host.c_str(), username.c_str(), password.c_str(), db.c_str(), 0, NULL, 0) == NULL) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to connect to MySQL", (char*)mysql_error(m_handle)));
        mysql_close(m_handle);
        return(false);
    }

    return(true);
}

void MySQLDataLoader::Close() {
    if (m_handle) {
        mysql_close(m_handle);
        m_handle = 0;
    }
}

Array<GigaObject*> MySQLDataLoader::GetObjects(std::string table, std::map<std::string, std::string> search) {
    Array<GigaObject*> retval;

    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();

    if (this->TableExists(table) == false) {
        return(retval);
    }

    this->UpdateTables();
    std::string query = "SELECT * FROM " + table;

    // Add where clauses
    if (search.size()) {
        query += " WHERE 1=1 ";
        std::map<std::string, std::string>::iterator it = search.begin();
        for (; it != search.end(); it++) {
            query += " AND " + it->first + " = '" + it->second + "'";
        }
    }

    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get record list from MySQL.", (char*)mysql_error(m_handle)));
        return(retval);
    }

    MYSQL_RES* result = mysql_store_result(m_handle);
    if (result == NULL) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get record list from MySQL result.", (char*)mysql_error(m_handle)));
        return(retval);
    }

    int colCount = mysql_field_count(m_handle);

    MYSQL_ROW row;
    MYSQL_FIELD* sqlfields = mysql_fetch_fields(result);
    std::string primaryKeyCol = table + "_id";
    std::vector<DataRecord*> records;
    while ((row = mysql_fetch_row(result))) {
        unsigned int primaryKeyID = 0;
        DataRecord* record = new DataRecord();
        
        for (int i = 0; i < colCount; i++) {
            printf("Setting %s to %s.\n", sqlfields[i].name, row[i]);
            if (strcmp(sqlfields[i].name, primaryKeyCol.c_str()) == 0) {
                primaryKeyID = atoi(row[i]);
                record->SetID(primaryKeyID);
                continue;
            }
            
            // Read type and value
            std::string fieldValue = row[i];
            int type = atoi(fieldValue.substr(0, fieldValue.find(":")).c_str());
            std::string value = fieldValue.substr(fieldValue.find(":") + 1);

            record->Set(std::string(sqlfields[i].name), std::string(row[i]), type);
        }

        records.push_back(record);
    }

    mysql_free_result(result);
    
    // Save records
    auto ri = m_records.find(table);
    if(ri == m_records.end()) {
        m_records[table] = std::map<uint32_t, DataRecord*>();
        ri = m_records.find(table);
    }
    
    auto it = records.begin();
    for(; it != records.end(); it++) {
        uint32_t id = (*it)->GetID();
        ri->second[id] = (*it);
    }

    // Process objects
    MetaSystem* metaSystem = GetSystem<MetaSystem>();

    auto ci = m_objectCache.find(table);
    if (ci == m_objectCache.end()) {
        m_objectCache[table] = std::map<uint32_t, GigaObject*>();
        ci = m_objectCache.find(table);
    }

    auto oi = records.begin();
    for (; oi != records.end(); oi++) {
        // Check cache for existing object
        GigaObject* obj = 0;
        uint32_t primaryKeyValue = (*oi)->GetID();
        auto obi = ri->second.find(primaryKeyValue);
        if (obi != ri->second.end()) {
            obj = obi->second;
        }
        else {
            obj = metaSystem->CreateClass(table);
            ci->second[primaryKeyValue] = obj;
        }

        // Deserialize data record
        obj->DeserializeObject(*oi);

        m_recordCache[obj] = *oi;
        retval.push_back(obj);
    }

    return(retval);
}


Array<DataRecord*> MySQLDataLoader::GetRecords(std::string table, std::map<std::string, std::string> search) {
    Array<DataRecord*> records;
    
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();

    if (this->TableExists(table) == false) {
        return(records);
    }

    this->UpdateTables();
    std::string query = "SELECT * FROM " + table;

    // Add where clauses
    if (search.size()) {
        query += " WHERE 1=1 ";
        std::map<std::string, std::string>::iterator it = search.begin();
        for (; it != search.end(); it++) {
            query += " AND " + it->first + " = '" + it->second + "'";
        }
    }

    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get record list from MySQL.", (char*)mysql_error(m_handle)));
        return(records);
    }

    MYSQL_RES* result = mysql_store_result(m_handle);
    if (result == NULL) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get record list from MySQL result.", (char*)mysql_error(m_handle)));
        return(records);
    }

    int colCount = mysql_field_count(m_handle);

    MYSQL_ROW row;
    MYSQL_FIELD* sqlfields = mysql_fetch_fields(result);
    std::string primaryKeyCol = table + "_id";
    while ((row = mysql_fetch_row(result))) {
        uint32_t primaryKeyID = 0;
        DataRecord* record = new DataRecord();
        
        for (int i = 0; i < colCount; i++) {
            printf("Setting %s to %s.\n", sqlfields[i].name, row[i]);
            if (strcmp(sqlfields[i].name, primaryKeyCol.c_str()) == 0) {
                primaryKeyID = atoi(row[i]);
                record->SetID(primaryKeyID);
                record->Set(std::string(sqlfields[i].name), std::to_string(primaryKeyID), Variant::VAR_INT32);
                continue;
            }
            
            // Read type and value
            std::string fieldValue = row[i];
            int type = atoi(fieldValue.substr(0, fieldValue.find(":")).c_str());
            std::string value = fieldValue.substr(fieldValue.find(":") + 1);

            record->Set(std::string(sqlfields[i].name), std::string(row[i]), type);
        }

        records.push_back(record);
    }

    mysql_free_result(result);
    
    // Save records
    auto ri = m_records.find(table);
    if(ri == m_records.end()) {
        m_records[table] = std::map<uint32_t, DataRecord*>();
        ri = m_records.find(table);
    }
    
    auto it = records.begin();
    for(; it != records.end(); it++) {
        uint32_t id = (*it)->GetID();
        ri->second[id] = (*it);
    }
    return(records);
}

void MySQLDataLoader::SaveRecords(std::string table, Array<DataRecord*> records) {
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();

    // If nothing to save, exit
    if (records.size() == 0) {
        return;
    }

    // We will use the first record to determine structure
    DataRecord* firstRecord = records[0];

    // Create a table if one does not exist
    std::string query = "CREATE TABLE IF NOT EXISTS ";
    query += table + " (";

    query += table + "_id INTEGER";

    Array<std::string> fields = firstRecord->GetKeys();
    auto fi = fields.begin();
    for (; fi != fields.end(); fi++) {
        query += "," + (*fi) + " VARCHAR(255)";
    }

    query += ", PRIMARY KEY(" + table + "_id ASC))";

    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to create table.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to create table.");
        return;
    }

    // Add any columns that may not yet exist
    query = "DESCRIBE " + table + "";
    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get schema.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to get schema.");
        return;
    }

    // Put the column names into a vector
    Array<std::string> existingCols;
    MYSQL_RES* result = mysql_store_result(m_handle);
    if (result == NULL) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to store MySQL result.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to store MySQL result.");
        return;
    }

    int colCount = mysql_field_count(m_handle);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        existingCols.push_back(row[0]);
    }
    
    mysql_free_result(result);

    fi = fields.begin();
    for (; fi != fields.end(); fi++) {
        auto it = existingCols.find((*fi));
        if (it == existingCols.end()) {
            query = "ALTER TABLE " + table + " ADD COLUMN " + (*fi) + " TEXT";

            if (mysql_query(m_handle, query.c_str())) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to alter table.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to alter DB.");
                return;
            }
        }
    }

    // Save records
    std::string primaryKeyCol = table + "_id";
    auto ri = records.begin();
    for (; ri != records.end(); ri++) {
        DataRecord* dr = *ri;

        // Process new record
        if (dr->GetID() == 0) {
            // Insert
            query = "INSERT INTO " + table + " (";
            std::string values = ") VALUES (";
            auto f = fields.begin();
            for (; f != fields.end(); f++) {
                std::string field = (*f);
                if (field.compare(primaryKeyCol) != 0) {
                    Variant* v = dr->Get(field);
                    std::string value = std::to_string(v->GetType()) + ":" + v->ToString();

                    if (f != fields.begin()) {
                        query += ",";
                        values += ",";
                    }
                    query += field;
                    values += "'" + value + "'";
                }
            }

            query += values + ")";

            if (mysql_query(m_handle, query.c_str())) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to insert object.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to insert object.");
            }

            unsigned int newID = (unsigned int)mysql_insert_id(m_handle);
            if (newID == 0) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to insert object.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to insert object.");
            }

            dr->SetID(newID);
            continue;
        }

        // Get the primary key
        char primaryKeyID[10];
        sprintf(primaryKeyID, "%d", dr->GetID());

        // Process deletion
        if (dr->IsDeleted()) {
            // Construct query
            query = "DELETE FROM " + table + " WHERE " + primaryKeyCol + " = ";
            query += primaryKeyID;

            if (mysql_query(m_handle, query.c_str())) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to delete object.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to delete object");
            }

            continue;
        }

        // Updated record
        query = "UPDATE " + table + " SET ";
        auto f = fields.begin();
        for (; f != fields.end(); f++) {
            std::string field = (*f);
            if (field.compare(primaryKeyCol) != 0) {
                Variant* v = dr->Get(*f);
                std::string value = std::to_string(v->GetType()) + ":" + dr->Get(field)->ToString();

                if (f != fields.begin()) query += ",";
                query += field + " = '" + value + "'";
            }
        }

        query += " WHERE " + primaryKeyCol + " = " + primaryKeyID;

        if (mysql_query(m_handle, query.c_str())) {
            errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to update object.", (char*)mysql_error(m_handle)));
            GIGA_ASSERT(false, "Unable to update object.");
        }
    }
}

void MySQLDataLoader::SaveObjects(std::string table, Array<GigaObject*> records) {
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();

    // If nothing to save, exit
    if (records.size() == 0) {
        return;
    }

    // We will use the first record to determine structure
    DataRecord* firstRecord = 0;
    auto di = m_recordCache.find(records[0]);
    if (di == m_recordCache.end()) {
        firstRecord = new DataRecord();
        records[0]->SerializeObject(firstRecord);
        m_recordCache[records[0]] = firstRecord;
    }
    else {
        firstRecord = di->second;
    }

    // Create a table if one does not exist
    std::string query = "CREATE TABLE IF NOT EXISTS ";
    query += table + " (";

    query += table + "_id INTEGER";

    Array<std::string> fields = firstRecord->GetKeys();
    auto fi = fields.begin();
    for (; fi != fields.end(); fi++) {
        query += ",'" + (*fi) + "' VARCHAR(255)";
    }

    query += ", PRIMARY KEY(" + table + "_id ASC))";

    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to create table.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to create table.");
        return;
    }

    // Add any columns that may not yet exist
    query = "DESCRIBE " + table + "";
    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to get schema.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to get schema.");
        return;
    }

    // Put the column names into a vector
    Array<std::string> existingCols;
    MYSQL_RES* result = mysql_store_result(m_handle);
    if (result == NULL) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to store MySQL result.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to store MySQL result.");
        return;
    }

    int colCount = mysql_field_count(m_handle);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        existingCols.push_back(row[0]);
    }
    
    mysql_free_result(result);

    fi = fields.begin();
    for (; fi != fields.end(); fi++) {
        auto it = existingCols.find((*fi));
        if (it == existingCols.end()) {
            query = "ALTER TABLE " + table + " ADD COLUMN " + (*fi) + " TEXT";

            if (mysql_query(m_handle, query.c_str())) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to alter table.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to alter DB.");
                return;
            }
        }
    }

    // Save records
    std::string primaryKeyCol = table + "_id";
    auto ri = records.begin();
    for (; ri != records.end(); ri++) {
        // Find an associated data record, or create one
        DataRecord* dr = 0;
        auto di = m_recordCache.find(*ri);
        if (di == m_recordCache.end()) {
            dr = new DataRecord();
            m_recordCache[*ri] = dr;
        }
        else {
            dr = di->second;
        }

        (*ri)->SerializeObject(dr);

        // Process new record
        if (dr->GetID() == 0) {
            // Insert
            query = "INSERT INTO " + table + " (";
            std::string values = ") VALUES (";
            auto f = fields.begin();
            for (; f != fields.end(); f++) {
                std::string field = (*f);
                if (field.compare(primaryKeyCol) != 0) {
                    Variant* v = dr->Get(field);
                    std::string value = std::to_string(v->GetType()) + ":" + dr->Get(field)->ToString();

                    if (f != fields.begin()) {
                        query += ",";
                        values += ",";
                    }
                    query += "'" + field + "'";
                    values += "'" + value + "'";
                }
            }

            query += values + ")";

            if (mysql_query(m_handle, query.c_str())) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to insert object.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to insert object.");
            }

            unsigned int newID = (unsigned int)mysql_insert_id(m_handle);
            if (newID == 0) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to insert object.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to insert object.");
            }

            dr->SetID(newID);
            continue;
        }

        // Get the primary key
        char primaryKeyID[10];
        sprintf(primaryKeyID, "%d", dr->GetID());

        // Process deletion
        if (dr->IsDeleted()) {
            // Construct query
            query = "DELETE FROM " + table + " WHERE " + primaryKeyCol + " = ";
            query += primaryKeyID;

            if (mysql_query(m_handle, query.c_str())) {
                errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to delete object.", (char*)mysql_error(m_handle)));
                GIGA_ASSERT(false, "Unable to delete object.");
            }

            continue;
        }

        // Updated record
        query = "UPDATE " + table + " SET ";
        auto f = fields.begin();
        for (; f != fields.end(); f++) {
            std::string field = (*f);
            if (field.compare(primaryKeyCol) != 0) {
                Variant* v = dr->Get(field);
                std::string value = std::to_string(v->GetType()) + ":" + dr->Get(field)->ToString();

                if (f != fields.begin()) query += ",";
                query += "'" + field + "' = '" + value + "'";
            }
        }

        query += " WHERE " + primaryKeyCol + " = " + primaryKeyID;

        if (mysql_query(m_handle, query.c_str())) {
            errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to update object.", (char*)mysql_error(m_handle)));
            GIGA_ASSERT(false, "Unable to update object.");
        }
    }
}

GigaObject* MySQLDataLoader::GetObject(std::string table, int primaryKey) {
    // First check our cache
    auto ci = m_objectCache.find(table);
    if (ci != m_objectCache.end()) {
        auto oi = m_objectCache[table].find(primaryKey);
        if (oi != m_objectCache[table].end()) return(oi->second);
    }

    std::map<std::string, std::string> search;
    search[table + "_id"] = std::to_string(primaryKey);

    Array<GigaObject*> records = this->GetObjects(table, search);
    if (records.size()) {
        return(records[0]);
    }

    return(0);
}

void MySQLDataLoader::Delete(std::string table, std::map<std::string, std::string> search) {
    if (this->TableExists(table) == false) {
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
    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to delete.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to delete.");
    }
}

bool MySQLDataLoader::TableExists(std::string name) {
    // Get error system
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    // Check if the table exists
    bool tableExists = false;
    std::string query = "SELECT * FROM information_schema.tables WHERE table_schema = '" + m_db + "' AND table_name = '" + name + "' LIMIT 1;";
    if (mysql_query(m_handle, query.c_str())) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to check table.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to check table.");
    }
    
    MYSQL_RES* result = mysql_store_result(m_handle);
    if (result == NULL) {
        errorSystem->HandleError(new Error(Error::MSG_WARN, "Unable to store MySQL result.", (char*)mysql_error(m_handle)));
        GIGA_ASSERT(false, "Unable to store MySQL result.");
    }
    
    tableExists = mysql_num_rows(result) > 0;
    mysql_free_result(result);
    
    return(tableExists);
}

void MySQLDataLoader::UpdateTables() {
    
}
