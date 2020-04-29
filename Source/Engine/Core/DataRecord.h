
#ifndef datarecord_h
#define datarecord_h

#include <Core/GigaObject.h>
#include <Core/Variant.h>

class GIGA_API DataRecord {
public:
    DataRecord();
    ~DataRecord();
    
    /**
     * Set a key/value pair
     */
    void Set(std::string var, Variant* val);
    void Set(std::string var, std::string val, uint32_t type);
    Variant* Get(std::string var);
    
    /**
     * Check whether a key exists
     */
    bool KeyExists(std::string var);
    
    /**
     * Serialize into binary
     */
    unsigned char* Serialize(int& size);
    void Deserialize(unsigned char* data, int& size);
    
    /**
     * Get/set updated sttus
     */
    void SetUpdated(bool updated) { m_updated = updated; }
    bool Updated() { return m_updated; }
    
    /**
     * Get/set record ID
     */
    void SetID(uint32_t key) { m_primaryKey = key; }
    uint32_t GetID() { return m_primaryKey; }
    
    /**
     * Get keys
     */
    std::vector<std::string> GetKeys();
    
    /**
     * Whether the object is marked for deletion
     */
    bool IsDeleted() { return m_deleted; }
    void MarkDeleted(bool deleted) { m_deleted = deleted; }
    
protected:
    // Variant variables
    std::map<std::string, Variant*> m_vars;
    
    // Original reference object
    GigaObject* m_obj;
    
    // Updated since last reset
    bool m_updated;
    
    // Data record ID (primary key)
    uint32_t m_primaryKey;
    
    // Deleted?
    bool m_deleted;
};

#endif
