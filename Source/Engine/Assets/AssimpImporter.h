
#ifndef assimpimporter_h
#define assimpimporter_h

#include <Render/Mesh.h>

class GIGA_API AssimpImporter {
public:
    AssimpImporter() = default;
    ~AssimpImporter() = default;

    Mesh* LoadMesh(std::string filename);
    void LoadAnimation(std::string filename, std::string animationName, Mesh* mesh);
};

#endif
