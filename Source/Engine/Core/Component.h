
#ifndef component_h
#define component_h

#include <Core/GigaObject.h>

/**
 * Base component class - components are added to Entity objects
 */
class GIGA_API Component : public GigaObject {
public:
    virtual ~Component() = default;
    
protected:
    Component() = default;
};

#endif
