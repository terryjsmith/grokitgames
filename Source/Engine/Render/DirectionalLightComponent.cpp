
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
        
        vector4 position = vector4(m_transform->GetWorldPosition(), 0.0f);
        camera->GetTransform()->SetLook(vector3(position));
        vector3 cameraPosition = sceneCamera->GetTransform()->GetWorldPosition();
        vector3 cameraUp = sceneCamera->GetTransform()->GetRight();
        vector3 cameraRight = sceneCamera->GetTransform()->GetUp();
        vector3 cameraLook = sceneCamera->GetTransform()->GetLook();
        
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
        
        // Determine sphere radius
        vector3 center = vector3(0.0f);
        float maxDistance = 0.0f;
        for(int k = 0; k < 8; k++) {
            center += frustumCorners[k];
            
            for(int j = 0; j < 8; j++) {
                maxDistance = std::max(maxDistance, glm::distance(frustumCorners[k], frustumCorners[j]));
            }
        }
        
        float radius = maxDistance / 2.0f;
        center /= 8.0f;

        float additionalOffset = 20.0f;
        vector3 lightCameraPosition = center - (vector3(position) * (maxDistance + additionalOffset));
        matrix4 viewMatrix = glm::lookAt(lightCameraPosition, center, vector3(0, 1, 0));
        
        // Create our light's orthographic projection matrix
        matrix4 lightProj = glm::ortho(-radius, radius, -radius, radius, 1.0f, (maxDistance * 2.0f) + additionalOffset);
        
        camera->SetProjectionMatrix(lightProj);
        camera->SetViewMatrix(viewMatrix);
        
        // Set "camera" and draw
        m_depthPass->SetTexture((Texture2D*)m_depthTextures[i]);
        m_depthPass->SetView(camera);
        m_depthPass->Render(scene);
        
        //m_depthTextures[i]->Save("depth.bmp");
    }
}

void DirectionalLightComponent::Serialize(DataRecord* record){
    TransformableComponent::Serialize(record);
    
    record->Set("Light.color", new Variant(m_color));
}

void DirectionalLightComponent::Deserialize(DataRecord* record) {
    TransformableComponent::Deserialize(record);
    
    m_color = record->Get("Light.color")->AsVector3();
}
