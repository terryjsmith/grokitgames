
#ifndef message_h
#define message_h

#include <Core/GigaObject.h>

/**
 * Message base class (for passing messages around)
 */
class GIGA_API Message : public GigaObject {
public:
    Message(uint32_t type, GigaObject* obj = 0);
    Message(std::string type, GigaObject* obj = 0);
    ~Message() = default;
    
    GIGA_CLASS_NAME("Message");
    
public:
    uint32_t type;
    
    // Object
    GigaObject* obj;
};

#endif
