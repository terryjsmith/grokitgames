
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
    
    /* Erase old messages
    if(count > 0) {
        auto it = m_messages.end();
        it--;
        
        for(; it != m_messages.begin(); it--) {
            if(it->second == true) {
                delete(it->first);
                m_messages.erase(it);
            }
        }
        
        it = m_messages.begin();
        if(it->second == true) {
            delete(it->first);
            m_messages.erase(it);
        }
    }*/
    
    
    // Dispatch
    auto it = m_messages.begin();
    for(; it != m_messages.end(); it++) {
        if(it->second == true) {
            continue;
        }
        
        Message* m = it->first;
        
        auto callback = m_handlers.begin();
        for (; callback != m_handlers.end(); callback++) {
            MessageHandler* cb = (*callback);
            if (cb->type == m->type) {
                cb->cb(cb->obj, m);
            }
        }
        
        m_messages[m] = true;
    }
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
    
    m_messages[msg] = false;
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
