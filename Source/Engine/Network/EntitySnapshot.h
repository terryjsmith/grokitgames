
#ifndef entitysnapshot_h
#define entitysnapshot_h

#include <Core/Entity.h>
#include <Core/DataRecord.h>

/**
 * A snapshot of data changes that have occured on entities in a specific tick
 */
class GIGA_API EntitySnapshot {
public:
    EntitySnapshot();
    ~EntitySnapshot();
    
    enum {
        SNAPSHOT_FULL = 1,
        SNAPSHOT_DELTA,
    };
    
    /**
     * Serialize entities and components into a byte stream, providing a buffer of bufferSize
     * Starting from offset entities in, updated to reflect current offset at completion
     */
    void Serialize();
    
    /**
     * Set data for later deserialization
     */
    void SetData(unsigned char* buffer, int size);
    
    /**
     * Get data
     */
    unsigned char* GetData(int& size);
    
    /**
     * Deserialize message
     */
    void Deserialize();
    
    /**
     * Add entity/components
     */
    void AddEntity(Entity* entity);
    void AddComponent(Component* component);
    
public:
    // Snapshot time (in ticks)
    int tick;
    
    // Snapshot type
    int type;
    
    struct ComponentRecord {
        uint32_t entityID;
        DataRecord* record;
    };
    
    // Data records (partial deserialization)
    std::map<uint32_t, DataRecord*> entityRecords;
    Array<ComponentRecord*> componentRecords;

protected:
    // Buffer data
    unsigned char* m_buffer;
    int m_bufferSize;
};

#endif
