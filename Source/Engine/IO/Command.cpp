
#include <IO/Command.h>
#include <Core/Message.h>
#include <Core/TimeSystem.h>
#include <IO/InputSystem.h>
#include <Core/MessageSystem.h>
#include <Core/Application.h>

std::map<std::string, uint32_t> Command::m_messageTypes;
int Command::m_lastCommandID = 0;

Command::Command() {
    m_state = 0;
    m_message = 0;
    m_commandID = ++m_lastCommandID;
    m_entityID = 0;
    m_tick = 0;
}

Command::Command(uint32_t state, uint32_t type, uint32_t entityID, uint32_t commandID, uint32_t tick) {
    m_state = state;
    m_message = type;
    m_commandID = commandID == 0 ? ++m_lastCommandID : commandID;
    m_entityID = entityID;
    m_tick = tick;
}

Command* Command::Start(std::string type, uint32_t entityID) {
    std::map<std::string, uint32_t>::iterator it = m_messageTypes.find(type);
    if (it == m_messageTypes.end()) {
        GIGA_ASSERT(false, "Unregistered command type.");
    }
    
    TimeSystem* timeSystem = GetSystem<TimeSystem>();
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    InputSystem* inputSystem = GetSystem<InputSystem>();
    
    Command* c = inputSystem->CreateCommand();
    c->m_state = Command::COMMAND_START;
    c->m_message = it->second;
    c->m_commandID = ++m_lastCommandID;
    c->m_entityID = entityID;
    c->m_tick = timeSystem->GetCurrentTick();

    messageSystem->Broadcast(new Message("COMMAND_START", c));
    
    return(c);
}

Command* Command::End(std::string type, uint32_t entityID) {
    std::map<std::string, uint32_t>::iterator it = m_messageTypes.find(type);
    if (it == m_messageTypes.end()) {
        GIGA_ASSERT(false, "Unregistered command type.");
    }
    
    TimeSystem* timeSystem = GetSystem<TimeSystem>();
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    InputSystem* inputSystem = GetSystem<InputSystem>();
    
    // No open command, end received before start, create one in an ended state
    Command* c = inputSystem->CreateCommand();
    c->m_state = Command::COMMAND_END;
    c->m_message = it->second;
    c->m_commandID = ++m_lastCommandID;
    c->m_entityID = entityID;
    c->m_tick = timeSystem->GetCurrentTick();

    messageSystem->Broadcast(new Message("COMMAND_END", c));
    
    return(0);
}

void Command::End() {
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    
    m_state = Command::COMMAND_END;
    messageSystem->Broadcast(new Message("COMMAND_END", this));
}

void Command::RegisterCommandType(std::string name, int typeID) {
    std::map<std::string, uint32_t>::iterator it = m_messageTypes.find(name);
    if (it != m_messageTypes.end()) {
        GIGA_ASSERT(false, "Command type name or typeID already registered.");
    }
    
    m_messageTypes[name] = typeID;
}
