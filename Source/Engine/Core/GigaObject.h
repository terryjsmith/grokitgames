
#ifndef gigaobject_h
#define gigaobject_h

#include <Core/Variant.h>
#include <mutex>

namespace Meta { class Class; }
class DataRecord;

typedef Variant*(*CallableFunction)(GigaObject* obj, int argc, Variant** argv);

/**
 * Base giga object class
 */
class GIGA_API GigaObject {
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
    Variant* Call(std::string func, int argc, Variant** argv);
    
    /**
     * Serialization
     */
    virtual void Serialize(DataRecord* record) { }
    virtual void Deserialize(DataRecord* record) { }
    
    /**
     * Get class type
     */
    Meta::Class* GetClass();
    
    /**
     * Lock/unlock mutex
     */
    void Lock();
    void Unlock();
    
protected:
    GigaObject();
    
    // Class type
    Meta::Class* m_mc;
    
    // Mutex
    std::mutex* m_mutex;
};

#endif
