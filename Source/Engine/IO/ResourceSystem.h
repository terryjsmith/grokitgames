
#ifndef resourcesystem_h
#define resourcesystem_h

#include <Core/System.h>
#include <IO/ResourceObject.h>

GIGA_CLASS(Singleton)
class GIGA_API ResourceSystem : public System {
public:
    ResourceSystem() = default;
    ~ResourceSystem();
    
    GIGA_CLASS_NAME("ResourceSystem");
    
    /**
     * Initialize system
     */
    void Initialize();
    
    /**
     * Shutdown - remove all open resources
     */
    void Shutdown();
    
    /**
     * Load a resource
     */
    GIGA_FUNCTION() ResourceObject* LoadResource(std::string filename, std::string type);
    
    /**
     * Add a directory/path to search to find files
     */
    GIGA_FUNCTION() void AddSearchPath(std::string path);
    
    /**
     * Iterate over search paths to find full path by filename
     */
    std::string FindResourcePath(std::string filename);
    
    /**
     * Register resource alias
     */
    void RegisterResourceType(std::string alias, std::string type);
    
protected:
    // A list of searchable paths
    std::vector<std::string> m_paths;
    
    // A list of registered resources
    std::vector<Resource*> m_resources;
    
    // Loaded resource objects
    std::map<Resource*, ResourceObject*> m_resourceObjects;
    
    // Resource aliases
    std::map<std::string, std::string> m_resourceTypes;
};


#endif
