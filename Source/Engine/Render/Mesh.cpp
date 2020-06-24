
#include <Render/Mesh.h>
#include <Render/AssimpImporter.h>

Mesh::Mesh() {
    renderable = 0;
    nodes = 0;
}

void Mesh::ProcessData() {
    if(this->GetResource()->extension == "fbx") {
        AssimpImporter* importer = new AssimpImporter();
        importer->LoadMesh(this->GetResource()->fullPath, this);
    }
}
