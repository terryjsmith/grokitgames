
#ifndef momosystem_h
#define momosystem_h

#include <Core/System.h>
#include <Scripting/ScriptObject.h>

class GIGA_API MonoSystem : public System {
public:
    MonoSystem() = default;
    ~MonoSystem() = default;
    
    virtual std::string GetGigaName() { return classType->name; }
        
    Variant* Call(std::string func, int argc, Variant** argv);

public:
    MonoClassDesc* classType;
};

#endif
