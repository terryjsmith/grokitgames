
#ifndef resourceobject_h
#define resourceobject_h

#include <Core/GigaObject.h>
#include <IO/Resource.h>

class GIGA_API ResourceObject : public GigaObject {
public:
    ResourceObject() = default;
    virtual ~ResourceObject();
    
    /**
     * Process loaded data
     */
    virtual void ProcessData() { }
    
    /**
     * Get resource
     */
    Resource* GetResource();
    
protected:
    /**
     * Initialize with a resource
     */
    void Instantiate(Resource* resource);
    
    friend class ResourceSystem;
    
protected:
    Resource* m_resource;
};

#endif
