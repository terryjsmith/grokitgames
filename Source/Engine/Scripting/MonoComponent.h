
#ifndef monocomponent_h
#define monocomponent_h

#include <Core/Component.h>
#include <Scripting/ScriptObject.h>

class GIGA_API MonoComponent : public Component {
public:
    MonoComponent() = default;
    ~MonoComponent() = default;
    
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
