
#ifndef scriptobject_h
#define scriptobject_h

#include <Core/GigaObject.h>
#include <Scripting/MonoObjects.h>

class ScriptingSystem;

class GIGA_API ScriptObject : public GigaObject {
public:
    ScriptObject();
    ~ScriptObject() = default;
    
    virtual std::string GetGigaName() { return classType->name; }
    
    Variant* Call(std::string func, int argc, Variant** argv);

public:
    // Class type
    MonoClassDesc* classType;
    
    // Remote object
    MonoObject* remote;
    
    // Local object
    GigaObject* local;
};

#endif
