
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
        ERROR_NONE = 0,
        ERROR_DEBUG,
        ERROR_INFO,
        ERROR_WARN,
        ERROR_FATAL
    };
    
public:
    int level;
    std::string message;
    std::string detail;
};

#endif
