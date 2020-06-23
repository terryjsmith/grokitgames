
#include <Render/MeshComponent.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>

MeshComponent::MeshComponent() {
    mesh = 0;
    animation = 0;
}

void MeshComponent::Initialize(Mesh* mesh) {
    if(mesh == 0) return;

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
    Component::Serialize(record);
    
    record->SetTypeHint("mesh", "Mesh");
    record->Set("mesh", new Variant(mesh));

    record->Set("transform", new Variant(transform));
    record->Set("applyLighting", new Variant(applyLighting));
}

void MeshComponent::Deserialize(DataRecord* record) {
    Component::Deserialize(record);
    
    record->SetTypeHint("mesh", "Mesh");
    Mesh* mesh = record->Get("mesh")->AsObject<Mesh*>();
    this->Initialize(mesh);
    
    this->applyLighting = record->Get("applyLighting")->AsBool();
    this->transform = record->Get("transform")->AsObject<Transform*>();
}
