
#ifndef messagesystem_h
#define messagesystem_h

#include <Core/System.h>
#include <Core/Message.h>

/**
 * Message handling callback
 */
typedef void(*MessageHandlingCallback)(GigaObject* obj, Message* message);

class GIGA_API MessageSystem : public System {
public:
    MessageSystem();
    ~MessageSystem();
    
    GIGA_CLASS_NAME("MessageSystem");
    
    /**
     * Update (process messages)
     */
    void Update(float delta);
    
    /**
     * Send a message to all registered listeners
     */
    void Broadcast(Message* msg, bool synchronized = false);
    
    /**
     * Register a message type
     */
    uint32_t RegisterMessageType(std::string name) {
        uint32_t newId = ++m_idCounter;
        m_idMapping[name] = newId;
        return(newId);
    }
    
    /**
     * Get registered type
     */
    uint32_t GetMessageType(std::string name);
    
    /**
     * Register a callback
     */
    void RegisterCallback(GigaObject* obj, std::string type, MessageHandlingCallback cb);
    
protected:
    // Current message queue (flushed after handling)
    std::map<Message*, bool> m_messages;
    
    // Message callbacks
    struct MessageHandler {
        MessageHandlingCallback cb;
        uint32_t type;
        GigaObject* obj;
    };
    
    Array<MessageHandler*> m_handlers;
    
    // Registered message types
    std::map<std::string, uint32_t> m_idMapping;
    
    // Message ID counter
    uint32_t m_idCounter;
};

#endif
