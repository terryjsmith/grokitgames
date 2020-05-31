
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
    record->Set("filename", new Variant(mesh->GetResource()->filename));
    record->Set("position", new Variant(transform->GetWorldPosition()));
    record->Set("rotation", new Variant(transform->GetWorldRotation()));
    record->Set("scaling", new Variant(transform->GetWorldScaling()));
}

void MeshComponent::Deserialize(DataRecord* record) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    Mesh* mesh = dynamic_cast<Mesh*>(resourceSystem->LoadResource(record->Get("filename")->AsString(), "Mesh"));
    if(mesh != this->mesh) {
        this->mesh = mesh;
    }
    transform->SetWorldPosition(record->Get("position")->AsVector3());
    transform->SetWorldRotation(record->Get("rotation")->AsQuaternion());
    transform->SetWorldScaling(record->Get("scaling")->AsVector3());
}
