
#include <Render/DirectionalLightComponent.h>
#include <Render/RenderSystem.h>
#include <Render/Defines.h>
#include <Core/Application.h>

void DirectionalLightComponent::Initialize() {
    m_passes = 3;
    m_type = LIGHT_DIRECTIONAL;
    m_attenuation = 100.0f;
    
    // Get link to render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Create cubemap shadow texture
    m_depthTextures = (Texture**)malloc(sizeof(Texture*) * m_passes);
    
    // Create 3 textures for various depth levels
    for(int i = 0; i < m_passes; i++) {
        Texture2D* depthTexture = renderSystem->CreateTexture2D();
        depthTexture->Initialize(1024, 1024, COLOR_DEPTH_COMPONENT24, COLOR_DEPTH_COMPONENT);
        m_depthTextures[i] = depthTexture;
    }
    
    // Initialize depth pass
    m_depthPass = new DepthPass();
    m_depthPass->Initialize(1024, 1024);
    
    // Initialize "camera"
    m_cameras = (CameraComponent**)malloc(sizeof(CameraComponent*) * 3);
    for(int i = 0; i < m_passes; i++) {
        CameraComponent* camera = new CameraComponent();
        camera->SetNear(0.1f);
        m_cameras[i] = camera;
    }
}

void DirectionalLightComponent::GenerateDepthTexture(Scene* scene) {
    // Split the camera frustum into pieces for Cascaded Shadow Mapping
    float distances[4] = {
        0.01f, 0.15, 0.40, 1.0f
    };
    
    CameraComponent* sceneCamera = scene->camera;
    
    Frustum lightFrustums[3];
    
    // Create the frustums
    for(int i = 0; i < 3; i++) {
        lightFrustums[i] = sceneCamera->CalculateFrustum(distances[i] * m_attenuation, distances[i + 1] * m_attenuation);
    }
    
    for(int i = 0; i < 3; i++) {
        // Get the far point
        CameraComponent* camera = GetCamera(i);
        
        float fnear = distances[i] * m_attenuation;
        float ffar = distances[i + 1] * m_attenuation;
        
        camera->SetNear(fnear);
        camera->SetFar(ffar);
        camera->SetFOV(90.0f);
        camera->SetAspectRatio(1.0f);
        
        vector4 position = vector4(transform->GetWorldPosition(), 0.0f);
        camera->transform->SetLook(vector3(position));
        
        float fov = sceneCamera->GetFOV();
        float aspect = sceneCamera->GetAspectRatio();
        
        float tanHalfVFOV = tanf(glm::radians(fov / 2.0f));
        float tanHalfHFOV = tanf((glm::radians((fov * aspect) / 2.0f)));

        float xn = fnear * tanHalfHFOV;
        float xf = ffar * tanHalfHFOV;
        float yn = fnear * tanHalfVFOV;
        float yf = ffar * tanHalfVFOV;
        
        vector3 frustumCorners[8];
        frustumCorners[0] = vector3(xn, yn, -fnear);
        frustumCorners[1] = vector3(-xn, yn, -fnear);
        frustumCorners[2] = vector3(xn, -yn, -fnear);
        frustumCorners[3] = vector3(-xn, -yn, -fnear);
        
        frustumCorners[4] = vector3(xf, yf, -ffar);
        frustumCorners[5] = vector3(-xf, yf, -ffar);
        frustumCorners[6] = vector3(xf, -yf, -ffar);
        frustumCorners[7] = vector3(-xf, -yf, -ffar);
        
        vector4 frustumCornersL[8];

        float minX = FLT_MAX;
        float maxX = -FLT_MAX;
        float minY = FLT_MAX;
        float maxY = -FLT_MAX;
        float minZ = FLT_MAX;
        float maxZ = -FLT_MAX;

        matrix4 viewMatrix = glm::lookAt(vector3(0, 0, 0), -transform->GetWorldPosition(), vector3(0, 1, 0));
        matrix4 camInverse = glm::inverse(sceneCamera->GetViewMatrix());
        
        vector4 worldPosition = camInverse * vector4(0.0f, 0.0f, 0.0f, 1.0f);
        worldPosition = viewMatrix * worldPosition;
        
        for (uint32_t j = 0 ; j < 8 ; j++) {
            vector4 vW = camInverse * vector4(frustumCorners[j], 1.0);
            frustumCornersL[j] = viewMatrix * vW;

            minX = std::min(minX, frustumCornersL[j].x);
            maxX = std::max(maxX, frustumCornersL[j].x);
            minY = std::min(minY, frustumCornersL[j].y);
            maxY = std::max(maxY, frustumCornersL[j].y);
            minZ = std::min(minZ, frustumCornersL[j].z);
            maxZ = std::max(maxZ, frustumCornersL[j].z);
        }
        
        //matrix4 ortho = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        matrix4 ortho = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        
        camera->SetProjectionMatrix(ortho);
        matrix4 view2 = camera->GetViewMatrix();
        
        matrix4 viewproj = ortho * view2;
        
        // Set "camera" and draw
        m_depthPass->SetTexture((Texture2D*)m_depthTextures[i]);
        m_depthPass->SetView(camera);
        m_depthPass->Render(scene);
        
        //m_depthTextures[i]->Save("depth.bmp");
    }
}
