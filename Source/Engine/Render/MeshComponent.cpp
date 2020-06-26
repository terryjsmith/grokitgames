
#include <Render/MeshComponent.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>
#include <Render/AssimpImporter.h>

MeshComponent::MeshComponent() {
    mesh = 0;
    animation = 0;
}

void MeshComponent::Initialize(Mesh* mesh) {
    if(mesh == 0) return;
    if(this->mesh == mesh) return;

    this->renderable = mesh->renderable;
    this->applyLighting = true;
    this->mesh = mesh;
    
    auto it = children.begin();
    for(; it != children.end(); it++) {
        delete(*it);
    }
    
    auto ri = renderable->children.begin();
    for(;ri != renderable->children.end(); ri++) {
        MeshComponent* mc = new MeshComponent();
        mc->renderable = (*ri);
        
        children.push_back(mc);
    }
}

void MeshComponent::Serialize(DataRecord* record) {
    TransformableComponent::Serialize(record);
    
    record->SetTypeHint("mesh", "Mesh");
    record->Set("mesh", new Variant(mesh));
    record->Set("applyLighting", new Variant(applyLighting));
}

void MeshComponent::Deserialize(DataRecord* record) {
    TransformableComponent::Deserialize(record);
    
    record->SetTypeHint("mesh", "Mesh");
    Mesh* m = record->Get("mesh")->AsObject<Mesh*>();
    this->Initialize(m);

    this->applyLighting = record->Get("applyLighting")->AsBool();
}
