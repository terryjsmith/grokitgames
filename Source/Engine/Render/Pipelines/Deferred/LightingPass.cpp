
#include <Render/Pipelines/Deferred/LightingPass.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>
#include <Render/LightComponent.h>
#include <Render/MaterialSystem.h>

LightingPass::LightingPass() {
    m_windowWidth = m_windowHeight = 0;
    m_vertexBuffer = 0;
    m_vertexFormat = 0;
    m_normalTexture = 0;
    m_positionTexture = 0;
    m_materialTexture = 0;
    m_null = 0;
}

void LightingPass::Initialize(int width, int height) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Framebuffer* fb = renderSystem->CreateFramebuffer();
    fb->Initialize();
    
    Texture2D* buffer = renderSystem->CreateTexture2D();
    buffer->Initialize(width, height, COLOR_RGB16F, COLOR_RGB);
    
    fb->AddTexture(buffer, FRAMEBUFFER_SLOT_0);
    
    m_framebuffers.push_back(fb);
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("ortho.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("lighting.fs", "Shader"));
    
    // Create a program
    if (m_program == 0) {
        m_program = renderSystem->CreateShaderProgram();
        m_program->Initialize(vshader, fshader);
    }
    
    m_windowWidth = width;
    m_windowHeight = height;
    
    // Populate our vertex buffer and type
    float box[] = {
        (float)width, 0, 1, 1,
        0, 0, 0, 1,
        (float)width, (float)height, 1, 0,
        0, (float)height, 0, 0,
    };
    
    m_vertexFormat = renderSystem->CreateVertexFormat();
    m_vertexFormat->Initialize();
    m_vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 2, 0);
    m_vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD0, 2, 2);
    
    m_vertexBuffer = renderSystem->CreateVertexBuffer();
    m_vertexBuffer->Create(m_vertexFormat, 4, box, false);
    
    m_null = renderSystem->CreateTexture3D();
    
    m_blurFilter = new GaussianBlur();
    m_blurFilter->Initialize("gaussian.fs", width, height);
    m_blurFilter->SetInputTexture(buffer);
}

void LightingPass::Render(Scene* scene) {
    // First, loop through all lights and render depth from the light's perspective
    auto it = scene->renderables.begin();
    for(; it != scene->renderables.end(); it++) {
        LightComponent* lc = dynamic_cast<LightComponent*>((*it));
        if(lc != 0) {
            lc->GenerateDepthTexture(scene);
            //lc->GetDepthTexture(0)->Save("depth.bmp");
        }
    }
    
    // Bind shader program
    m_program->Bind();
    
    // Then render the data to the lighting texture
    m_framebuffers[0]->Bind();
    
    // Get render system, disable depth test
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->DisableDepthTest();
    
    // Set cumulative blending
    renderSystem->EnableBlending();
    renderSystem->SetBlendFunc(BLEND_ONE, BLEND_ONE);
    
    renderSystem->Clear(COLOR_BUFFER_BIT);
    
    // Get matrices
    matrix4 proj = glm::ortho(0.0f, (float)m_windowWidth, (float)m_windowHeight, 0.0f);
    m_program->Set("ortho", proj);
    
    // Get the far plane distance
    CameraComponent* camera = scene->camera;
    m_program->Set("cameraPosition", camera->transform->GetWorldPosition());
    
    // Get view matrix
    matrix4 view = camera->GetViewMatrix();
    matrix4 worldViewInverse = glm::inverse(view);
    m_program->Set("worldviewInverseMatrix", worldViewInverse);
    
    m_positionTexture->Bind(6);
    m_program->Set("texturePosition", 6);
    
    m_normalTexture->Bind(1);
    m_program->Set("textureNormal", 1);
    
    //m_normalTexture->Save("normal.bmp");
    
    m_materialTexture->Bind(2);
    m_program->Set("textureMaterial", 2);
    
    //m_materialTexture->Save("material.bmp");
    
    // Get our material texture
    MaterialSystem* materialSystem = GetSystem<MaterialSystem>();
    Texture* materialTexture = materialSystem->GetTexture();
    
    materialTexture->Bind(3);
    m_program->Set("textureMaterialLookup", 3);
    materialTexture->SetTextureFilter(FILTER_NEAREST);
    
    m_vertexFormat->Bind();
    m_vertexBuffer->Bind();
    
    // Enable the attributes we need
    m_vertexFormat->EnableAttribute(0, VERTEXTYPE_ATTRIB_POSITION);
    m_vertexFormat->EnableAttribute(3, VERTEXTYPE_ATTRIB_TEXCOORD0);
    
    // Loop through each light
    // First, loop through all lights and render depth from the light's perspective
    it = scene->renderables.begin();
    for(; it != scene->renderables.end(); it++) {
        LightComponent* lc = dynamic_cast<LightComponent*>((*it));
        if(lc == 0) continue;
        
        CameraComponent* cc = lc->GetCamera(0);
        
        // Bind vars
        m_program->Set("lightPosition", lc->transform->GetWorldPosition());
        m_program->Set("lightColour", lc->GetColor());
        m_program->Set("lightType", lc->GetType());
        m_program->Set("farPlane", cc->GetFar());
        m_program->Set("nearPlane", cc->GetNear());
        m_program->Set("lightDirection", lc->transform->GetLook());
    
        if(lc->GetType() == LightComponent::LIGHT_POINT) {
            lc->GetDepthTexture(0)->Bind(4);
            m_program->Set("lightShadowMap3D", 4);
            
            // Render
            renderSystem->Draw(DRAW_TRIANGLE_STRIP, 4);
        }
        
        if(lc->GetType() == LightComponent::LIGHT_SPOT) {
            // Get light "camera"
            CameraComponent* cc = lc->GetCamera(0);
            matrix4 viewproj = cc->GetProjectionMatrix();
            viewproj = viewproj * cc->GetViewMatrix();
            
            m_program->Set("lightAngle", glm::cos(glm::radians(cc->GetFOV() / 2.0f)));
            m_program->Set("lightSpaceMatrix[0]", viewproj);
            
            lc->GetDepthTexture(0)->Bind(4);
            m_program->Set("lightShadowMap[0]", 4);
            
            // Render
            renderSystem->Draw(DRAW_TRIANGLE_STRIP, 4);
        }
        
        if(lc->GetType() == LightComponent::LIGHT_DIRECTIONAL) {
            //for(int i = 0; i < 3; i++) {
                lc->GetDepthTexture(0)->Bind(5);
                lc->GetDepthTexture(0)->SetTextureFilter(FILTER_LINEAR);
                m_program->Set("lightShadowMap", 5);
            
                CameraComponent* cc = lc->GetCamera(0);
                matrix4 viewproj = cc->GetProjectionMatrix() * cc->GetViewMatrix();
                m_program->Set("lightSpaceMatrix", viewproj);

                m_program->Set("farPlane", cc->GetFar());
                m_program->Set("cascadeIndex", 0);
                
                // Render
                renderSystem->Draw(DRAW_TRIANGLE_STRIP, 4);
            //}
        }
        
        //m_positionTexture->Bind(0);
        //m_program->Set("texturePosition", 0);
    }
    
    renderSystem->DisableBlending();
    
    m_normalTexture->Unbind();
    m_program->Unbind();
    
    m_vertexBuffer->Unbind();
    m_vertexFormat->Unbind();
    
    //m_blurFilter->Apply();
    //m_blurFilter->GetOutputTexture()->Save("blur.bmp");
    //m_framebuffers[0]->GetTexture(0)->Save("lighting.bmp");
}

Texture2D* LightingPass::GetOutputTexture() {
    return(m_blurFilter->GetOutputTexture());
}
