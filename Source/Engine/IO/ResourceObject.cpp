
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

void ResourceObject::Serialize(DataRecord* record) {
    record->SetTypeHint("resource", "Resource");
    record->Set("resource", new Variant(m_resource));
}

void ResourceObject::Deserialize(DataRecord* record) {
    record->SetTypeHint("resource", "Resource");
    m_resource = record->Get("resource")->AsObject<Resource*>();
    this->ProcessData();
}

std::string ResourceObject::ToString() {
    return(m_resource->filename);
}
