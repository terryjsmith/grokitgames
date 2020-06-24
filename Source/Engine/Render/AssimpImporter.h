
#ifndef assimpimporter_h
#define assimpimporter_h

#include <Render/Mesh.h>

class GIGA_API AssimpImporter {
public:
    AssimpImporter() = default;
    ~AssimpImporter() = default;

    void LoadMesh(std::string filename, Mesh* mesh);
    void LoadAnimation(std::string filename, std::string animationName, Mesh* mesh);
};

#endif
