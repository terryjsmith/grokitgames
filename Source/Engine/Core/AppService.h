
#ifndef appservice_h
#define appservice_h

#include <Core/GigaObject.h>

GIGA_CLASS()
class GIGA_API AppService : public GigaObject {
public:
    AppService() = default;
    virtual ~AppService() = default;
    
    GIGA_CLASS_NAME("AppService");
};

#endif
