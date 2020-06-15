
#ifndef monoobjects_h
#define monoobjects_h

#include <Core/Giga.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

struct MonoMethodDesc {
    std::string name;
    MonoMethod* method;
};

struct MonoFieldDesc {
    std::string name;
    MonoClassField* field;
};

struct MonoClassDesc {
    std::string name;
    MonoClass* _class;
    std::map<std::string, MonoFieldDesc*> fields;
    std::map<std::string, MonoMethodDesc*> methods;
};

#endif
