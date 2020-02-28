
#include <Core/Message.h>
#include <Core/MessageSystem.h>
#include <Core/Application.h>

Message::Message(uint32_t type, GigaObject* obj) {
    this->type = type;
    this->obj = obj;
}

Message::Message(std::string type, GigaObject* obj) {
    MessageSystem* messageSystem = GetSystem<MessageSystem>();
    uint32_t itype = messageSystem->GetMessageType(type);
    
    GIGA_ASSERT(itype != 0, "Type not registered.");
    
    this->type = itype;
    this->obj = obj;
}
