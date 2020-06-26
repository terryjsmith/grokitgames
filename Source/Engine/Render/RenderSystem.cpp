
#include <Render/RenderSystem.h>
#include <Core/MetaSystem.h>
#include <Core/Application.h>
#include <Core/World.h>
#include <Render/RenderComponent.h>
#include <Render/CameraComponent.h>
#include <IO/Profiler.h>

RenderSystem::RenderSystem() {
    m_fullscreen = false;
    m_scene = new Scene();
    m_scene->camera = new CameraComponent(); // Create a default camera
    
    m_ambient = vector3(0.25f, 0.25f, 0.25f);
    m_triCount = 0;
}

RenderSystem::~RenderSystem() {
    delete(m_scene);
}

void RenderSystem::Initialize() {

}

void RenderSystem::Update(float delta) {
    PROFILE_START_AREA("RenderSystem");
    m_triCount = 0;
    
    // Get all render components from scene
    World* world = World::GetInstance();
    m_scene->renderables.clear();
    m_scene->renderables = world->FindComponents<RenderComponent>();

    // Get camera object
    auto cameras = world->FindComponents<CameraComponent>();
    if(cameras.size()) {
        m_scene->camera = cameras[0];
    }
    
    m_renderPipeline->Render(m_scene);
    PROFILE_END_AREA("RenderSystem");
}
