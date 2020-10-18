
#ifndef serializer_h
#define serializer_h

#include <IO/MemoryReader.h>
#include <IO/MemoryWriter.h>
#include <Core/DataRecord.h>

class GIGA_API Serializer {
public:
    Serializer() = default;
    ~Serializer() = default;
    
    /**
     * Serialize into binary
     */
    static unsigned char* Serialize(DataRecord* record, uint32_t& size);
    static void Deserialize(DataRecord* record, unsigned char* data, uint32_t& size);
    
    /**
     * Get size of binary record
     */
    static unsigned int GetBinarySize(DataRecord* record);
};

#endif
