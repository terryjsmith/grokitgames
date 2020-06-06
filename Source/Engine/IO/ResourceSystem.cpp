
#include <IO/ResourceSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>

ResourceSystem::~ResourceSystem() {
    auto it = m_resources.begin();
    for(; it != m_resources.end(); it++) {
        delete(*it);
    }
    m_resources.clear();
}

void ResourceSystem::Initialize() {

}

ResourceObject* ResourceSystem::LoadResource(std::string filename, std::string type) {
    MetaSystem* metaSystem = GetSystem<MetaSystem>();
    Resource* resource = 0;
    
    auto ti = m_resourceTypes.find(type);
    if(ti != m_resourceTypes.end()) {
        type = ti->second;
    }
    
    // Check to see if we've loaded this resource
    auto it = m_resources.begin();
    for (; it != m_resources.end(); it++) {
        if ((*it)->filename == filename) {
            resource = (*it);
            break;
        }
    }
    
    // If we have a resource, we probably also have a resource object to return
    std::map<Resource*, ResourceObject*>::iterator ri = m_resourceObjects.find(resource);
    if (ri != m_resourceObjects.end()) {
        return(ri->second);
    }
    
    // If not, find and initialize the resource
    if (resource == 0) {
        // First, get the full path by searching out search paths
        std::string fullpath = FindResourcePath(filename);
        if (fullpath.empty()) {
            // Error
            GetSystem<ErrorSystem>()->HandleError(new Error(Error::MSG_WARN, "Unable to locate resource file", filename));
            return(0);
        }
        
        Resource* r = new Resource();
        r->Initialize(fullpath, Resource::FILEMODE_READ | Resource::FILEMODE_BINARY);
        r->type = type;
        
        m_resources.push_back(r);
        resource = r;
    }
    
    // Create a new ResourceObject of the necessary type
    ResourceObject* obj = dynamic_cast<ResourceObject*>(metaSystem->CreateClass(type));
    GIGA_ASSERT(obj != 0, "Resource object not found.");
    
    // Create an instance of the Resource into the ResourceObject
    obj->Instantiate(resource);
    m_resourceObjects[resource] = obj;
    
    return(obj);
}

void ResourceSystem::AddSearchPath(std::string path) {
    m_paths.push_back(path);
}

std::string ResourceSystem::FindResourcePath(std::string filename) {
    std::string ret;
    
    // First try to open the file with the path it's on now
    if (Resource::Exists(filename)) {
        return(filename);
    }
    
    // Loop through our list of files, try to open this one
    auto i = m_paths.begin();
    for (; i != m_paths.end(); i++) {
        std::string fn((*i));
        fn += "/";
        fn += filename;
        
        if (Resource::Exists(fn)) {
            return(fn);
        }
    }
    
    return(ret);
}

void ResourceSystem::Shutdown() {
    // Check to see if we've loaded this resource
    auto it = m_resources.begin();
    for (; it != m_resources.end(); it++) {
        delete(*it);
    }
    
    m_resources.clear();
}

void ResourceSystem::RegisterResourceType(std::string alias, std::string type) {
    m_resourceTypes[alias] = type;
}
