
#include <Render/DirectionalLightComponent.h>
#include <Render/RenderSystem.h>
#include <Render/Defines.h>
#include <Core/Application.h>

#define DIRECTIONAL_LIGHT_TEXSIZE 1024
#define DIRECTIONAL_LIGHT_PASSES    4

void DirectionalLightComponent::Initialize() {
    m_passes = DIRECTIONAL_LIGHT_PASSES;
    m_type = LIGHT_DIRECTIONAL;
    m_attenuation = 100.0f;
    
    // Get link to render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Create cubemap shadow texture
    m_depthTextures = (Texture**)malloc(sizeof(Texture*) * m_passes);
    
    // Create 3 textures for various depth levels
    for(int i = 0; i < m_passes; i++) {
        Texture2D* depthTexture = renderSystem->CreateTexture2D();
        depthTexture->Initialize(DIRECTIONAL_LIGHT_TEXSIZE, DIRECTIONAL_LIGHT_TEXSIZE, COLOR_DEPTH_COMPONENT24, TEXTURE_TYPE_FLOAT, COLOR_DEPTH_COMPONENT);
        m_depthTextures[i] = depthTexture;
    }
    
    // Initialize depth pass
    m_depthPass = new DepthPass();
    m_depthPass->Initialize(DIRECTIONAL_LIGHT_TEXSIZE, DIRECTIONAL_LIGHT_TEXSIZE);
    
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
    float distances[5] = {
        0.01f, 0.10, 0.30, 0.60, 1.0f
    };
    
    CameraComponent* sceneCamera = scene->camera;
    float sceneFar = sceneCamera->GetFar();
    
    Frustum lightFrustums[DIRECTIONAL_LIGHT_PASSES];
    
    // Create the frustums
    for(int i = 0; i < m_passes; i++) {
        float nearDistance = distances[i] * sceneFar;
        if(i == 0) {
            nearDistance = 0.1f;
        }
        lightFrustums[i] = sceneCamera->CalculateFrustum(nearDistance, distances[i + 1] * sceneFar);
    }
    
    for(int i = 0; i < m_passes; i++) {
        // Get the far point
        CameraComponent* camera = GetCamera(i);
        
        float nearDistance = distances[i] * sceneFar;
        if(i == 0) {
            nearDistance = 0.1f;
        }
        
        float fnear = nearDistance;
        float ffar = distances[i + 1] * sceneFar;
        
        camera->SetNear(fnear);
        camera->SetFar(ffar);
        camera->SetFOV(90.0f);
        camera->SetAspectRatio(1.0f);
        
        vector4 position = vector4(transform->GetWorldPosition(), 0.0f);
        camera->transform->SetLook(vector3(position));
        vector3 cameraPosition = sceneCamera->transform->GetWorldPosition();
        vector3 cameraUp = sceneCamera->transform->GetRight();
        vector3 cameraRight = sceneCamera->transform->GetUp();
        vector3 cameraLook = sceneCamera->transform->GetLook();
        
        float fov = sceneCamera->GetFOV();
        float aspect = sceneCamera->GetAspectRatio();
        
        float tanHalfVFOV = tanf(glm::radians(fov / 2.0f));
        float tanHalfHFOV = tanf((glm::radians((fov * aspect) / 2.0f)));

        float xn = fnear * tanHalfHFOV;
        float xf = ffar * tanHalfHFOV;
        float yn = fnear * tanHalfVFOV;
        float yf = ffar * tanHalfVFOV;
        
        vector3 frustumCorners[8];
        
        // Near plane
        frustumCorners[0] = cameraPosition + (cameraLook * fnear) + (cameraUp * yn) - (cameraRight * xn); // Top left
        frustumCorners[1] = cameraPosition + (cameraLook * fnear) + (cameraUp * yn) + (cameraRight * xn); // Top right
        frustumCorners[2] = cameraPosition + (cameraLook * fnear) - (cameraUp * yn) - (cameraRight * xn); // Bottom left
        frustumCorners[3] = cameraPosition + (cameraLook * fnear) - (cameraUp * yn) + (cameraRight * xn); // Bottom right
        
        // Far plane
        frustumCorners[4] = cameraPosition + (cameraLook * ffar) + (cameraUp * yf) - (cameraRight * xf); // Top left
        frustumCorners[5] = cameraPosition + (cameraLook * ffar) + (cameraUp * yf) + (cameraRight * xf); // Top right
        frustumCorners[6] = cameraPosition + (cameraLook * ffar) - (cameraUp * yf) - (cameraRight * xf); // Bottom left
        frustumCorners[7] = cameraPosition + (cameraLook * ffar) - (cameraUp * yf) + (cameraRight * xf); // Bottom right
        
        vector3 position2 = vector3(0.0f);
        for(int k = 0; k < 8; k++) {
            position2 += frustumCorners[k];
        }
        
        position2 /= 8.0f;
        
        vector3 position3 = position2 + vector3(-position * (ffar / 2.0f));
        
        vector4 frustumCornersL[8];

        float minX = FLT_MAX;
        float maxX = -FLT_MAX;
        float minY = FLT_MAX;
        float maxY = -FLT_MAX;
        float minZ = FLT_MAX;
        float maxZ = -FLT_MAX;

        matrix4 viewMatrix = glm::lookAt(position3, position2, vector3(0, 1, 0));
        matrix4 viewMatrix2 = glm::lookAt(position2 + vector3(position.x, position.y, position.z), position2, vector3(0, -1, 0));
        
        for (uint32_t j = 0 ; j < 8 ; j++) {
            vector4 vW = vector4(frustumCorners[j], 1.0);
            frustumCornersL[j] = viewMatrix2 * vW;

            minX = std::min(minX, frustumCornersL[j].x);
            maxX = std::max(maxX, frustumCornersL[j].x);
            minY = std::min(minY, frustumCornersL[j].y);
            maxY = std::max(maxY, frustumCornersL[j].y);
            minZ = std::min(minZ, frustumCornersL[j].z);
            maxZ = std::max(maxZ, frustumCornersL[j].z);
        }
        
        //vector3 halfExtents = vector3((maxX - minX) / 2.0f, (maxY - minY) / 2.0f, (std::abs(maxZ - minZ)) / 2.0f);
        vector3 halfExtents = vector3((maxX - minX), (maxY - minY), std::abs(-maxZ - -minZ));
        //halfExtents /= 2.0f;
        
        // Create our light's orthographic projection matrix
        matrix4 lightProj = glm::ortho(-halfExtents.x, halfExtents.x, -halfExtents.y, halfExtents.y, halfExtents.z, -halfExtents.z);

        //matrix4 ortho = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        matrix4 ortho = glm::ortho(minX, maxX, minY, maxY, maxZ, minZ);
        
        camera->SetProjectionMatrix(lightProj);
        camera->SetViewMatrix(viewMatrix2);
        
        matrix4 viewproj = ortho * viewMatrix;
        
        // Set "camera" and draw
        m_depthPass->SetTexture((Texture2D*)m_depthTextures[i]);
        m_depthPass->SetView(camera);
        m_depthPass->Render(scene);
        
        //m_depthTextures[i]->Save("depth.bmp");
    }
}
