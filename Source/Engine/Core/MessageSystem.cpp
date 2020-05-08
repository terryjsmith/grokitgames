
#include <Core/MessageSystem.h>

MessageSystem::MessageSystem() {
    m_idCounter = 0;
}

MessageSystem::~MessageSystem() {
    auto it = m_handlers.begin();
    for(; it != m_handlers.end(); it++) {
        delete(*it);
    }
    
    m_handlers.clear();
}

void MessageSystem::Update(float delta) {
    int count = (int)m_messages.size();
    
    for (int i = 0; i < count; i++) {
        Message* m = m_messages[i];
        
        auto callback = m_handlers.begin();
        for (; callback != m_handlers.end(); callback++) {
            MessageHandler* cb = (*callback);
            if (cb->type == m->type) {
                cb->cb(cb->obj, m);
            }
        }
        
        // delete(m);
    }
    
    // Erase as many messages as we had when we started - sometimes messages can spawn other messages
    // m_messages.erase(m_messages.begin(), m_messages.begin() + count);
}

void MessageSystem::Broadcast(Message* msg, bool synchronized) {
    // Ensure the type is set
    if (msg->type == 0) {
        std::map<std::string, uint32_t>::iterator it = m_idMapping.find(msg->GetGigaName());
        GIGA_ASSERT(it != m_idMapping.end(), "Unregistered message type.");
        
        msg->type = it->second;
    }
    
    if (synchronized) {
        // Iterate over all listeners and dispatch, no queueing
        auto callback = m_handlers.begin();
        for (; callback != m_handlers.end(); callback++) {
            MessageHandler* cb = (*callback);
            if (cb->type == msg->type) {
                cb->cb(cb->obj, msg);
            }
        }
        
        return;
    }
    
    m_messages.push_back(msg);
}

void MessageSystem::RegisterCallback(GigaObject* obj, std::string type, MessageHandlingCallback cb) {
    // Get the mapping
    std::map<std::string, uint32_t>::iterator it = m_idMapping.find(type);
    GIGA_ASSERT(it != m_idMapping.end(), "Message mapping not found.");
    
    MessageHandler* handler = new MessageHandler();
    handler->type = it->second;
    handler->cb = cb;
    handler->obj = obj;
    
    m_handlers.push_back(handler);
}

uint32_t MessageSystem::GetMessageType(std::string name) {
    std::map<std::string, uint32_t>::iterator it = m_idMapping.find(name);
    if(it == m_idMapping.end())
        return(0);
    
    return(it->second);
}
