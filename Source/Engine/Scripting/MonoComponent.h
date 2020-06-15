
#ifndef monocomponent_h
#define monocomponent_h

#include <Core/Component.h>
#include <Scripting/ScriptObject.h>

class GIGA_API MonoComponent : public Component {
public:
    MonoComponent() = default;
    ~MonoComponent() = default;
    
    virtual std::string GetGigaName() { return classType->name; }
            
    Variant* Call(std::string func, int argc, Variant** argv);

public:
    MonoClassDesc* classType;
};

#endif 
