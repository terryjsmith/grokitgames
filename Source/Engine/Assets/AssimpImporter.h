
#ifndef assimpimporter_h
#define assimpimporter_h

#include <Render/Mesh.h>

class GIGA_API AssimpImporter {
public:
    AssimpImporter() = default;
    ~AssimpImporter() = default;

    Mesh* LoadFromFile(std::string filename);
};

#endif
