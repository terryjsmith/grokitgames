
#ifndef gigaobject_h
#define gigaobject_h

#include <Core/Variant.h>

namespace Meta { class Class; }
class DataRecord;

typedef Variant*(*CallableFunction)(GigaObject* obj, int argc, Variant** argv);

/**
 * Base giga object class
 */
class GigaObject {
public:
    virtual ~GigaObject() = default;
    
    /**
     * Pure virtual function that specifies string version of class name
     * Use GIGA_CLASS_NAME("name") to set
     */
    virtual std::string GetGigaName() = 0;
    
    /**
    * Call a function on this object by passing a variable number of variants
    */
    Variant Call(std::string func, int argc, Variant** argv);
    
    /**
     * Serialization
     */
    void Serialize(DataRecord* record);
    void Deserialize(DataRecord* record);
    
    /**
     * Get class type
     */
    Meta::Class* GetClass();
    
protected:
    GigaObject() = default;
    
    // Class type
    Meta::Class* m_mc;
};

#endif
