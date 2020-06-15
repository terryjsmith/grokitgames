
#include "register_globals.h"
#include <IO/Keyboard.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>
#include <IO/Command.h>

void RegisterGlobals() {
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    
    /* Register keyboard
    scriptingSystem->SetGlobal("INPUTDEVICE_KEYBOARD", new Variant(INPUTDEVICE_KEYBOARD));
    
    // Commands
    scriptingSystem->SetGlobal("COMMAND_START", new Variant(Command::COMMAND_START));
    scriptingSystem->SetGlobal("COMMAND_END", new Variant(Command::COMMAND_END));
    
    // Register keys
    scriptingSystem->SetGlobal("KEY_UP", new Variant(KEY_UP));
    scriptingSystem->SetGlobal("KEY_DOWN", new Variant(KEY_DOWN));
    scriptingSystem->SetGlobal("KEY_LEFT", new Variant(KEY_LEFT));
    scriptingSystem->SetGlobal("KEY_RIGHT", new Variant(KEY_RIGHT));
    scriptingSystem->SetGlobal("KEY_SPACE", new Variant(KEY_SPACE));
    
    scriptingSystem->SetGlobal("KEY_A", new Variant(KEY_A));
    scriptingSystem->SetGlobal("KEY_B", new Variant(KEY_B));
    scriptingSystem->SetGlobal("KEY_C", new Variant(KEY_C));
    scriptingSystem->SetGlobal("KEY_D", new Variant(KEY_D));
    scriptingSystem->SetGlobal("KEY_E", new Variant(KEY_E));
    scriptingSystem->SetGlobal("KEY_F", new Variant(KEY_F));
    scriptingSystem->SetGlobal("KEY_G", new Variant(KEY_G));
    scriptingSystem->SetGlobal("KEY_H", new Variant(KEY_H));
    scriptingSystem->SetGlobal("KEY_I", new Variant(KEY_I));
    scriptingSystem->SetGlobal("KEY_J", new Variant(KEY_J));
    scriptingSystem->SetGlobal("KEY_K", new Variant(KEY_K));
    scriptingSystem->SetGlobal("KEY_L", new Variant(KEY_L));
    scriptingSystem->SetGlobal("KEY_M", new Variant(KEY_M));
    scriptingSystem->SetGlobal("KEY_N", new Variant(KEY_N));
    scriptingSystem->SetGlobal("KEY_O", new Variant(KEY_O));
    scriptingSystem->SetGlobal("KEY_P", new Variant(KEY_P));
    scriptingSystem->SetGlobal("KEY_Q", new Variant(KEY_Q));
    scriptingSystem->SetGlobal("KEY_R", new Variant(KEY_R));
    scriptingSystem->SetGlobal("KEY_S", new Variant(KEY_S));
    scriptingSystem->SetGlobal("KEY_T", new Variant(KEY_T));
    scriptingSystem->SetGlobal("KEY_U", new Variant(KEY_U));
    scriptingSystem->SetGlobal("KEY_V", new Variant(KEY_V));
    scriptingSystem->SetGlobal("KEY_W", new Variant(KEY_W));
    scriptingSystem->SetGlobal("KEY_X", new Variant(KEY_X));
    scriptingSystem->SetGlobal("KEY_Y", new Variant(KEY_Y));
    scriptingSystem->SetGlobal("KEY_Z", new Variant(KEY_Z));*/

}
