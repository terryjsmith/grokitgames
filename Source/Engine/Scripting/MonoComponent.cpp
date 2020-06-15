
#include <Scripting/MonoComponent.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>

Variant* MonoComponent::Call(std::string func, int argc, Variant** argv) {
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    return(scriptingSystem->CallFunction(this, func, argc, argv));
}

