
#include <Scripting/MonoComponent.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>

Variant* MonoComponent::Call(std::string func, int argc, Variant** argv) {
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    return(scriptingSystem->CallFunction(this, func, argc, argv));
}

void MonoComponent::Serialize(DataRecord* record) {
    Variant* args[1];
    args[0] = new Variant(record);
    this->Call("Serialize", 1, args);
}

void MonoComponent::Deserialize(DataRecord* record) {
    Variant* args[1];
    args[0] = new Variant(record);
    this->Call("Deserialize", 1, args);
}
