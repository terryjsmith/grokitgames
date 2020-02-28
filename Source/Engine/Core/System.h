
#ifndef system_h
#define system_h

#include <Core/GigaObject.h>

class GIGA_API System : public GigaObject {
public:
    System() = default;
    virtual ~System() = default;
    
    GIGA_CLASS_NAME("System");
    
    virtual void Initialize() { }
    virtual void Update(float delta) { }
    virtual void Shutdown() { }
};

#endif
