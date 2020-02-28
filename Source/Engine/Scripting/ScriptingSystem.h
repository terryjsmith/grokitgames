
#ifndef scriptingsystem_h
#define scriptingsystem_h

#include <Core/System.h>

GIGA_CLASS(Singleton)
class GIGA_API ScriptingSystem : public System {
public:
    ScriptingSystem() = default;
    ~ScriptingSystem() = default;
    
    GIGA_CLASS_NAME("ScriptingSystem");
};

#endif
