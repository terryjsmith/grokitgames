
#include <IO/ResourceObject.h>
#include <Core/DataRecord.h>

ResourceObject::~ResourceObject() {
    
}

void ResourceObject::Instantiate(Resource* resource) {
    m_resource = resource;
    
    // Load
    m_resource->Load();
    
    // Call data processor
    this->ProcessData();
    
    // Unload
    m_resource->Unload();
}

Resource* ResourceObject::GetResource() {
    return(m_resource);
}

std::string ResourceObject::ToString() {
    return(m_resource->filename);
}
