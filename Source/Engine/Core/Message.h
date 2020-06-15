
#ifndef message_h
#define message_h

#include <Core/GigaObject.h>

/**
 * Message base class (for passing messages around)
 */
GIGA_CLASS()
class GIGA_API Message : public GigaObject {
public:
    Message() : type(0), obj(0) { }
    Message(uint32_t type, GigaObject* obj = 0);
    Message(std::string type, GigaObject* obj = 0);
    ~Message() = default;
    
    GIGA_CLASS_NAME("Message");
    
    /**
     * Get details
     */
    GIGA_FUNCTION() uint32_t GetMessageType() { return type; }
    GIGA_FUNCTION() GigaObject* GetObject() { return obj; }
    
public:
    uint32_t type;
    
    // Object
    GigaObject* obj;
};


#endif

