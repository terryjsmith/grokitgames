
#ifndef error_h
#define error_h

#include <Core/GigaObject.h>

class GIGA_API Error : public GigaObject {
public:
    Error() = default;
    Error(int level, std::string message, std::string detail = "");
    ~Error() = default;
    
    GIGA_CLASS_NAME("Error");
    
    enum {
        MSG_NONE = 0,
        MSG_DEBUG,
        MSG_INFO,
        MSG_WARN,
        MSG_FATAL
    };
    
public:
    int level;
    std::string message;
    std::string detail;
};

#endif
