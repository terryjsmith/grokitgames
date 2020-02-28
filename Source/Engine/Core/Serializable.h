
#ifndef serializable_h
#define serializable_h

#include <Core/DataRecord.h>

class GIGA_API Serializable {
public:
    Serializable() = default;
    ~Serializable() = default;
    
    /**
     * Serialize / deserialize
     */
    virtual void Serialize(DataRecord* record) = 0;
    virtual void Deserialize(DataRecord* record) = 0;
};

#endif
