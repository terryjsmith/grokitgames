
#ifndef monoentity_h
#define monoentity_h

#include <Core/Entity.h>
#include <Scripting/ScriptObject.h>

class GIGA_API MonoEntity : public Entity {
public:
    MonoEntity() = default;
    ~MonoEntity() = default;
    
    virtual std::string GetGigaName() { return classType->name; }
            
    Variant* Call(std::string func, int argc, Variant** argv);

public:
    MonoClassDesc* classType;
};

#endif
