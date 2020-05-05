
#include "register_globals.h"
#include <IO/Keyboard.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>

void RegisterGlobals() {
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    
    // Register keyboard
    scriptingSystem->SetGlobal("INPUTDEVICE_KEYBOARD", new Variant(INPUTDEVICE_KEYBOARD));
    
    // Register keys
    scriptingSystem->SetGlobal("KEY_UP", new Variant(KEY_UP));
    scriptingSystem->SetGlobal("KEY_DOWN", new Variant(KEY_DOWN));
    scriptingSystem->SetGlobal("KEY_LEFT", new Variant(KEY_LEFT));
    scriptingSystem->SetGlobal("KEY_RIGHT", new Variant(KEY_RIGHT));
}
