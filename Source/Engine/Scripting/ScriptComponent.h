
#ifndef scriptcomponent_h
#define scriptcomponent_h

#include <Core/Component.h>
#include <Scripting/Script.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>


GIGA_CLASS()
class GIGA_API ScriptComponent : public Component {
public:
    ScriptComponent();
    ~ScriptComponent() = default;
    
    GIGA_CLASS_NAME("ScriptComponent");
    
    
    /**
     * Initialize from a source script
     */
    void Initialize(Script* script);

    /**
     * Call a function inside this component
     */
    void CallFunction(std::string function, int argc, Variant** argv);

    /**
     * Cached function for quick calling
     */
    struct ScriptFunction {
        std::string funcName;
        MonoMethod* func;
    };

    /**
     * Set a global variable inside this script
     */
    void SetGlobal(std::string name, Variant* value);
    
protected:
    // Our script
     Script* m_scriptSource;

    // List of functions from inside of the script
    std::vector<ScriptFunction*> m_functions;
};

#endif
