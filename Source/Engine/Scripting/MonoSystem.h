
#ifndef momosystem_h
#define momosystem_h

#include <Core/System.h>
#include <Scripting/ScriptObject.h>

class GIGA_API MonoSystem : public System {
public:
    MonoSystem() = default;
    ~MonoSystem() = default;
    
    // Get class name from Mono class desc instead
    virtual std::string GetGigaName() { return classType->name; }
        
    // Call a function through our meta API (but via Mono)
    Variant* Call(std::string func, int argc, Variant** argv);
    
    // Serialize/deserialize
    void Serialize(DataRecord* record);
    void Deserialize(DataRecord* record);

public:
    MonoClassDesc* classType;
};

#endif
