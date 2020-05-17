
#ifndef component_h
#define component_h

#include <Core/GigaObject.h>

class Entity;

/**
 * Base component class - components are added to Entity objects
 */
class GIGA_API Component : public GigaObject {
public:
    virtual ~Component() = default;
    
    /**
     * Get parent entity
     */
    Entity* GetParent() { return m_parent; }
    
    friend class Entity;
    
protected:
    Component();
    
    // Parent entity
    Entity* m_parent;
};

#endif
