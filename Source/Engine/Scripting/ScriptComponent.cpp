
#include <Scripting/ScriptComponent.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>
#include <Core/ErrorSystem.h>

ScriptComponent::ScriptComponent() {
    m_scriptSource = 0;
}

void ScriptComponent::Initialize(Script* script) {
    // Get our systems
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    // Add globals
    
    // Add classes

    // Copy source
    std::string source = m_scriptSource->GetResource()->GetString();

    // Get the init function

}

void ScriptComponent::SetGlobal(std::string name, Variant* value) {
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();

}

void ScriptComponent::CallFunction(std::string function, int argc, Variant** argv) {
    // Check to ensure this function exists
    ScriptFunction* scriptFunction = 0;
    for (size_t i = 0; i < m_functions.size(); i++) {
        if (m_functions[i]->funcName == function) {
            scriptFunction = m_functions[i];
        }
    }
    
    if (scriptFunction == 0) return;
    
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();

}
