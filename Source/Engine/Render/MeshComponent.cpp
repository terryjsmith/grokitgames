
#include <Render/MeshComponent.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>
#include <Render/AssimpImporter.h>

MeshComponent::MeshComponent() {
    mesh = 0;
    animation = 0;
    instanced = false;
    instances = 0;
}

void MeshComponent::Initialize(Mesh* mesh) {
    this->renderable = mesh->renderable;
    this->mesh = mesh;
    
    auto it = children.begin();
    for(; it != children.end(); it++) {
        delete(*it);
    }
    children.clear();
    
    auto ri = renderable->children.begin();
    for(;ri != renderable->children.end(); ri++) {
        MeshComponent* mc = new MeshComponent();
        mc->renderable = (*ri);
        
        children.push_back(mc);
    }
}

void MeshComponent::Serialize(DataRecord* record) {
    TransformableComponent::Serialize(record);

    record->Set("Mesh.applyLighting", new Variant(applyLighting));
    record->Set("Mesh.mesh", new Variant(mesh == 0 ? std::string() : mesh->GetResource()->filename));
    
    //record->Set("program", new Variant(program));
}

void MeshComponent::Deserialize(DataRecord* record) {
    TransformableComponent::Deserialize(record);
    
    this->applyLighting = record->Get("Mesh.applyLighting")->AsBool();
    std::string filename = record->Get("Mesh.mesh")->AsString();
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    Mesh* m = (Mesh*)resourceSystem->LoadResource(filename, "Mesh");
    this->Initialize(m);

    //this->program = record->Get("program")->AsObject<ShaderProgram*>();
}
