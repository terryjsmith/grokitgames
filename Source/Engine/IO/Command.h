
#ifndef command_h
#define command_h

#include <Core/Message.h>

class InputSystem;

GIGA_CLASS()
class GIGA_API Command : public GigaObject {
public:
    Command();
    Command(uint32_t state, uint32_t type, uint32_t entityID = 0, uint32_t commandID = 0, uint32_t tick = 0);
    ~Command() = default;
    
    GIGA_CLASS_NAME("Command");
    
    enum {
        COMMAND_START = 1,
        COMMAND_END = 0
    };
    
    /**
     * Initialize a new command
     */
    GIGA_FUNCTION() static Command* Start(std::string type, uint32_t entityID = 0);
    
    /**
     * End a command
     */
    GIGA_FUNCTION() static Command* End(std::string type, uint32_t entityID = 0);
    
    /**
     * Register a new command type
     */
    GIGA_FUNCTION() static void RegisterCommandType(std::string name, int typeID);
    
    /**
     * Get current command ID
     */
    static int GetLastCommandID() { return m_lastCommandID; }
    
    /**
     * Set state
     */
    void End();
    
    /**
     * Getters
     */
    GIGA_FUNCTION() int GetType() { return m_message; }
    GIGA_FUNCTION() int GetState() { return m_state; }
    GIGA_FUNCTION() int GetEntityID() { return m_entityID; }
    GIGA_FUNCTION() int GetCommandID() { return m_commandID; }
    GIGA_FUNCTION() int GetTick() { return m_tick; }
    
    friend class InputSystem;
    
protected:
    // Start or end?
    uint32_t m_state;
    
    // Command message
    uint32_t m_message;
    
    // The command ID
    uint32_t m_commandID;
    
    // Registered message types
    static std::map<std::string, uint32_t> m_messageTypes;
    
    // Last issued command ID
    static int m_lastCommandID;
    
    // Entity ID
    uint32_t m_entityID;
    
    // Tick time
    uint32_t m_tick;
};

#endif
