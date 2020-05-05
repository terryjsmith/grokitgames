
#include <Render/RenderSystem.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>
#include <Core/World.h>
#include <Render/RenderComponent.h>

RenderSystem::RenderSystem() {
    m_fullscreen = false;
    m_scene = new Scene();
    m_ambient = vector3(0.25f, 0.25f, 0.25f);
    m_triCount = 0;
}

RenderSystem::~RenderSystem() {
    delete(m_scene);
}

void RenderSystem::Initialize() {

}

void RenderSystem::Update(float delta) {
    m_triCount = 0;
    
    // Get all render components from scene
    World* world = World::GetInstance();
    m_scene->renderables.clear();
    m_scene->renderables = world->FindComponents<RenderComponent>();
    
    m_renderPipeline->Render(m_scene);
}
