
#ifndef scriptcomponent_h
#define scriptcomponent_h

#include <Core/Component.h>

GIGA_CLASS()
class GIGA_API ScriptComponent : public Component {
public:
    ScriptComponent() = default;
    ~ScriptComponent() = default;
    
    GIGA_CLASS_NAME("ScriptComponent");
    
public:
    // What type of class are we (for lookup)
    std::string className;
};

#endif
