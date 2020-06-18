
#include <Render/MeshComponent.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>

MeshComponent::MeshComponent() {
    mesh = 0;
    animation = 0;
}

void MeshComponent::Initialize(Mesh* mesh) {
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
    
    record->Set("filename", new Variant(mesh->GetResource()->filename));
    record->Set("transform", new Variant(transform));
    record->Set("applyLighting", new Variant(applyLighting));
}

void MeshComponent::Deserialize(DataRecord* record) {
    Component::Deserialize(record);
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();

    Mesh* mesh = dynamic_cast<Mesh*>(resourceSystem->LoadResource(record->Get("filename")->AsString(), "Mesh"));
    if(mesh != this->mesh) {
        this->mesh = mesh;
    }
    
    this->applyLighting = record->Get("applyLighting")->AsBool();
    this->transform = record->Get("transform")->AsObject<Transform*>();
}
