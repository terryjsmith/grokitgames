
#ifndef networksystem_h
#define networksystem_h

#include <Core/System.h>

GIGA_CLASS(Singleton)
class GIGA_API NetworkSystem : public System {
public:
    NetworkSystem() = default;
    ~NetworkSystem() = default;
    
    GIGA_CLASS_NAME("NetworkSystem");
    
    GIGA_FUNCTION() bool Connect(std::string host, int port);
};

#endif
