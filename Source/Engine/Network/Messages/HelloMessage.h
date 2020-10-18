
#ifndef hellomessage_h
#define hellomessage_h

#include <Network/NetworkMessage.h>

class GIGA_API HelloMessage : public NetworkMessage {
public:
    HelloMessage() = default;
    ~HelloMessage() = default;
    
    void OnSend();
    void OnReceive();
    
public:
    std::string sessionCode;
};

#endif
