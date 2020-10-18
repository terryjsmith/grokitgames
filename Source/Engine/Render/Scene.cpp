
#include <Render/Scene.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>

vector3 Scene::Unproject(vector2 screenCoords) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    RenderPipeline* pipeline = renderSystem->GetRenderPipeline();
    
    // Get the depth texture
    Texture2D* depthTexture = pipeline->GetDepthTexture();
    int textureWidth = depthTexture->GetWidth();
    int textureHeight = depthTexture->GetHeight();

    float* data = (float*)depthTexture->GetData();
    
    int dindex = (screenCoords.y * textureWidth) + screenCoords.x;
    float depth = data[dindex];
    free(data);
    
    // Get camera to determine near and far plane
    CameraComponent* camera = this->camera;
    matrix4 projMatrix = camera->GetProjectionMatrix();
    matrix4 viewMatrix = camera->GetViewMatrix();
    vector4 viewport = vector4(0, 0, textureWidth, textureHeight);

    vector3 objectCoords = glm::unProject(vector3(screenCoords.x, screenCoords.y, depth), viewMatrix, projMatrix, viewport);
    return(objectCoords);
}

vector2 Scene::Project(vector3 worldCoords) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    RenderPipeline* pipeline = renderSystem->GetRenderPipeline();
    
    // Get the depth texture
    Texture2D* depthTexture = pipeline->GetDepthTexture();
    int textureWidth = depthTexture->GetWidth();
    int textureHeight = depthTexture->GetHeight();
    
    CameraComponent* camera = this->camera;
    matrix4 projMatrix = camera->GetProjectionMatrix();
    matrix4 viewMatrix = camera->GetViewMatrix();
    float fnear = camera->GetNear();
    float ffar = camera->GetFar();
    
    vector4 screenCoords = projMatrix * viewMatrix * vector4(worldCoords, 1.0f);
    vector3 clipCoords = vector3(screenCoords) / screenCoords.w;
    vector3 windowCoords = vector3((float)textureWidth / 2.0 * clipCoords.x + (float)textureWidth / 2.0,
                                   (float)textureHeight / 2.0 * clipCoords.y + (float)textureHeight / 2.0,
                                   ffar - fnear / 2.0 * clipCoords.z + ffar + fnear  / 2.0);
    return(vector2(windowCoords));
}
