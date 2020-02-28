
#include <Render/TerrainComponent.h>
#include <IO/JSONDataLoader.h>
#include <IO/ResourceSystem.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>

TerrainComponent::TerrainComponent() {
    m_splat = 0;
    m_numTextures = 0;
    m_textures[0] = m_textures[1] = m_textures[2] = m_textures[3] = 0;
}

void TerrainComponent::Load(std::string terrain, vector3 offset) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Load JSON
    JSONDataLoader* loader = new JSONDataLoader();
    loader->Open(terrain);
    std::vector<DataRecord*> records = loader->GetRecords("terrains");
    
    // Load the heightmap texture
    Texture2D* hmaptex = (Texture2D*)resourceSystem->LoadResource(records[0]->Get("heightmap")->AsString(), "Texture2D");
    void* data = hmaptex->GetData();
    
    // Load textures
    m_numTextures = records[0]->Get("textures")->Size();
    for(int i = 0; i < m_numTextures; i++) {
        Variant* textures = records[0]->Get("textures");
        Variant* v = (*textures)[i];
        std::string filename = v->AsString();
        m_textures[i] = (Texture2D*)resourceSystem->LoadResource(filename, "Texture2D");
    }
    
    // Create the "root" terrain quad
    TerrainQuad* quad = new TerrainQuad();
    quad->Load(hmaptex, vector2(0), vector2(hmaptex->GetWidth()));
    
    // Save
    renderable = quad;
    transform->SetWorldPosition(offset);
    
    free(data);
    delete hmaptex;
}

void TerrainComponent::Unload() {
    if(renderable) {
        delete(renderable);
        renderable = 0;
    }
}

BoundingBox* TerrainComponent::GetBoundingBox() {
    TerrainQuad* quad = dynamic_cast<TerrainQuad*>(renderable);
    return(quad->GetBoundingBox());
}

void TerrainComponent::Update(Scene* scene) {
    CameraComponent* camera = scene->camera;
    TerrainQuad* quad = dynamic_cast<TerrainQuad*>(renderable);
    quad->SetLOD(camera->transform->GetWorldPosition());
}
