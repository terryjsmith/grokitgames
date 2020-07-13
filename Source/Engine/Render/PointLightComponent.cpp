
#include <Render/PointLightComponent.h>
#include <Render/RenderSystem.h>
#include <Render/Defines.h>
#include <Core/Application.h>

void PointLightComponent::Initialize() {
    m_passes = 6;
    m_type = LIGHT_POINT;
    
    // Get link to render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Create cubemap shadow texture
    m_depthTextures = (Texture**)malloc(sizeof(Texture*));
    
    Texture3D* depthTexture = renderSystem->CreateTexture3D();
    depthTexture->Initialize(POINTLIGHT_TEXTURE_SIZE, POINTLIGHT_TEXTURE_SIZE, COLOR_DEPTH_COMPONENT, TEXTURE_TYPE_FLOAT, COLOR_DEPTH_COMPONENT);
    m_depthTextures[0] = depthTexture;
    
    // Initialize depth pass
    m_depthPass = new DepthPass();
    m_depthPass->Initialize(POINTLIGHT_TEXTURE_SIZE, POINTLIGHT_TEXTURE_SIZE);
    
    // Initialize "camera"
    m_cameras = (CameraComponent**)malloc(sizeof(CameraComponent*));
    CameraComponent* camera = new CameraComponent();
    camera->SetNear(0.1f);
    m_cameras[0] = camera;
    
    // Right
    m_vectors[0].look = vector3(1, 0, 0);
    m_vectors[0].up = vector3(0, -1, 0);
    m_vectors[0].right = vector3(0, 0, -1);
    
    // Left
    m_vectors[1].look = vector3(-1, 0, 0);
    m_vectors[1].up = vector3(0, -1, 0);
    m_vectors[1].right = vector3(0, 0, 1);
    
    // Up
    m_vectors[2].look = vector3(0, 1, 0);
    m_vectors[2].up = vector3(0, 0, -1);
    m_vectors[2].right = vector3(1, 0, 0);
    
    // Down
    m_vectors[3].look = vector3(0, -1, 0);
    m_vectors[3].up = vector3(0, 0, -1);
    m_vectors[3].right = vector3(1, 0, 0);
    
    // Backward
    m_vectors[4].look = vector3(0, 0, 1);
    m_vectors[4].up = vector3(0, -1, 0);
    m_vectors[4].right = vector3(1, 0, 0);
    
    // Forward
    m_vectors[5].look = vector3(0, 0, -1);
    m_vectors[5].up = vector3(0, -1, 0);
    m_vectors[5].right = vector3(-1, 0, 0);
}

void PointLightComponent::GenerateDepthTexture(Scene* scene) {
    // Get our 3D texture
    Texture3D* texture = (Texture3D*)m_depthTextures[0];
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Get world position
    vector3 position = this->GetTransform()->GetWorldPosition();
    
    // Update "camera"
    CameraComponent* camera = GetCamera(0);
    camera->GetTransform()->SetWorldPosition(position);
    camera->SetFar(m_attenuation);
    camera->SetFOV(90.0f);
    camera->SetAspectRatio(1.0f);
    
    // Loop over each pass
    for(int i = 0; i < m_passes; i++) {
        m_depthPass->SetTexture(texture, texture->GetTarget(i));
        
        // Calculate frustum and matrices
        camera->GetTransform()->SetLook(m_vectors[i].look);
        camera->GetTransform()->SetUp(m_vectors[i].up);
        camera->GetTransform()->SetRight(m_vectors[i].right);
        
        // Set "camera" and draw
        m_depthPass->SetView(camera);
        m_depthPass->Render(scene);
    }
    
    //texture->Save("depth.bmp");
}

matrix4 PointLightComponent::GetLightMatrix() {
    return(matrix4(1.0f));
}

void PointLightComponent::Serialize(DataRecord* record) {
    TransformableComponent::Serialize(record);
    
    record->Set("Light.color", new Variant(m_color));
    record->Set("Light.attenuation", new Variant(m_attenuation));
}

void PointLightComponent::Deserialize(DataRecord* record) {
    TransformableComponent::Deserialize(record);
    
    m_color = record->Get("Light.color")->AsVector3();
    m_attenuation = record->Get("Light.attenuation")->AsFloat();
}
