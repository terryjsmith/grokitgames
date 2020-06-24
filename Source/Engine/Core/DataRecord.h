
#ifndef datarecord_h
#define datarecord_h

#include <Core/GigaObject.h>
#include <Core/Variant.h>

GIGA_CLASS()
class GIGA_API DataRecord : public GigaObject {
public:
    DataRecord();
    ~DataRecord();
    
    GIGA_CLASS_NAME("DataRecord");
    
    /**
     * Set a key/value pair
     */
    GIGA_FUNCTION() void Set(std::string var, Variant* val);
    void Set(std::string var, std::string val, uint32_t type);
    GIGA_FUNCTION() Variant* Get(std::string var);
    
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
    Array<std::string> GetKeys();
    
    /**
     * Whether the object is marked for deletion
     */
    bool IsDeleted() { return m_deleted; }
    void MarkDeleted(bool deleted) { m_deleted = deleted; }
    
    /**
     * Type hint for objects that don't exist yet
     */
    struct TypeHint {
        std::string type;
        bool single;
    };
    
    /**
     * Set/get type hints (mostly used in editor)
     */
    void SetTypeHint(std::string field, std::string type, bool single = true);
    TypeHint* GetTypeHint(std::string field);
    
protected:
    // Variant variables
    std::map<std::string, Variant*> m_vars;
    
    // Type hints
    std::map<std::string, TypeHint*> m_types;
    
    // Updated since last reset
    bool m_updated;
    
    // Data record ID (primary key)
    uint32_t m_primaryKey;
    
    // Deleted?
    bool m_deleted;
};

#endif
