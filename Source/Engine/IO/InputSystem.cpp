
#include <IO/InputSystem.h>
#include <Core/Application.h>
#include <Core/MessageSystem.h>
#include <Core/MetaSystem.h>
#include <IO/Keyboard.h>
#include <IO/Command.h>
#include <IO/Profiler.h>

InputSystem::~InputSystem() {
    for(size_t i = 0; i < m_mappings.size(); i++) {
        delete m_mappings[i];
    }
    
    auto it = m_commands.begin();
    for(; it != m_commands.end(); it++) {
        delete(*it);
    }
    m_commands.clear();
}

void InputSystem::Initialize() {
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    messageSystem->RegisterMessageType("COMMAND_START");
    messageSystem->RegisterMessageType("COMMAND_END");
    
    messageSystem->RegisterMessageType("KEY_PRESS");
    messageSystem->RegisterMessageType("KEY_RELEASE");
}

void InputSystem::Update(float delta) {
    PROFILE_START_AREA("InputSystem");
    
    for(size_t i = 0; i < m_devices.size(); i++) {
        m_devices[i]->Update();
    }
    
    PROFILE_END_AREA("InputSystem");
}

void InputSystem::RegisterInputDevice(InputDevice* device) {
    int type = device->GetType();
    for(size_t i = 0; i < m_devices.size(); i++) {
        if(m_devices[i]->GetType() == type) {
            GIGA_ASSERT(false, "There is already an input device of this type registered.");
        }
    }
    
    m_devices.push_back(device);
}

InputDevice* InputSystem::GetInputDevice(int type) {
    for(size_t i = 0; i < m_devices.size(); i++) {
        if(m_devices[i]->GetType() == type) {
            return(m_devices[i]);
        }
    }
    
    return(0);
}

void InputSystem::RegisterInputMapping(InputDevice* device, int button, std::string command) {
    // Do we already have something generating this command? If so, re-map
    for(size_t i = 0; i < m_mappings.size(); i++) {
        if(m_mappings[i]->command == command) {
            m_mappings[i]->device = device;
            m_mappings[i]->button = button;
            return;
        }
    }
    
    // Otherwise create a new mapping
    InputMapping* mapping = new InputMapping();
    mapping->device = device;
    mapping->button = button;
    mapping->command = command;
    
    m_mappings.push_back(mapping);
}

std::string InputSystem::FindInputMapping(InputDevice* device, int button) {
    std::string command;
    for(size_t i = 0; i < m_mappings.size(); i++) {
        if(m_mappings[i]->device == device && m_mappings[i]->button == button) {
            command = m_mappings[i]->command;
            break;
        }
    }
    
    return(command);
}

Command* InputSystem::CreateCommand() {
    Command* cmd = new Command();
    m_commands.push_back(cmd);
    return(cmd);
}

Command* InputSystem::CreateCommand(uint32_t state, uint32_t type, uint32_t entityID, uint32_t commandID, uint32_t tick) {
    Command* cmd = new Command(state, type, entityID, commandID, tick);
    m_commands.push_back(cmd);
    return(cmd);
}
