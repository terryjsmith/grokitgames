
#include <Render/TerrainSystem.h>
#include <Render/RenderSystem.h>
#include <Core/World.h>
#include <Core/Application.h>

TerrainSystem::TerrainSystem() {
    m_maxQuadSize = 64;
    m_loadDistance = 200;
    m_maxLOD = 4;
    m_terrainScale = 0.5f;
}

void TerrainSystem::Update(float delta) {
    // Get the scene
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Scene* scene = renderSystem->GetScene();
    
    // Get all render components from scene
    World* world = World::GetInstance();
    std::vector<TerrainComponent*> components = world->FindComponents<TerrainComponent>();
    auto it = components.begin();
    for(; it != components.end(); it++) {
        (*it)->Update(scene);
    }
}
