
#include <Render/DepthPass.h>
#include <Render/RenderSystem.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>

void DepthPass::Initialize(int width, int height) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    m_framebuffer = renderSystem->CreateFramebuffer();
    m_framebuffer->Initialize();
    
    m_depthTexture = renderSystem->CreateTexture2D();
    m_depthTexture->Initialize(width, height, COLOR_DEPTH_COMPONENT, TEXTURE_TYPE_FLOAT, COLOR_DEPTH_COMPONENT);
    
    m_framebuffer->AddTexture(m_depthTexture, FRAMEBUFFER_SLOT_DEPTH);
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("depth.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("depth.fs", "Shader"));
    
    // Create a program
    if (m_program == 0) {
        m_program = renderSystem->CreateShaderProgram();
        m_program->Initialize(vshader, fshader);
    }
}

void DepthPass::SetTexture(Texture2D* tex) {
    m_framebuffer->SetTexture(tex, 0, FRAMEBUFFER_SLOT_DEPTH);
}

void DepthPass::SetTexture(Texture3D* tex, int slot) {
    m_framebuffer->SetTexture(tex, FRAMEBUFFER_SLOT_DEPTH, slot);
}

void DepthPass::Render(Scene* scene) {
    // Use our program
    m_program->Bind();
    
    // Bind our FBO and set the viewport to the proper size
    m_framebuffer->Bind();
    
    // Get render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->EnableDepthTest(TEST_LEQUAL);
    //renderSystem->EnableFaceCulling(CULLMODE_BACK);
    
    // Clear our buffer
    renderSystem->SetClearColor(vector4(1, 1, 1, 1));
    renderSystem->Clear(DEPTH_BUFFER_BIT);
    
    // Tell the system depth write only
    renderSystem->SetDrawBuffer(DRAW_BUFFER_NONE);
    
    // Get matrices
    matrix4 view = m_camera->GetViewMatrix();
    matrix4 proj = m_camera->GetProjectionMatrix();
    
    matrix4 viewproj = proj * view;
    
    /*float shadowMapSize = 1024.0f; // Set this to the size of your shadow map
    vector3 shadowOrigin = vector3(viewproj * vector4(0));
    shadowOrigin *= (shadowMapSize / 2.0f);
    vector2 roundedOrigin = vector2(std::round(shadowOrigin.x), std::round(shadowOrigin.y));
    vector2 rounding = roundedOrigin - vector2(shadowOrigin);
    rounding /= (shadowMapSize / 2.0f);
    matrix4 roundMatrix = glm::translate(matrix4(1.0f), vector3(rounding.x, rounding.y, 0.0f));
    
    viewproj *= roundMatrix;*/
    
    m_program->Set("projectionMatrix", proj);
    m_program->Set("farPlane", m_camera->GetFar());
    m_program->Set("lightPosition", m_camera->transform->GetWorldPosition());
    
    // Iterate over renderables
    auto it = scene->renderables.begin();
    for(; it != scene->renderables.end(); it++) {
        RenderComponent* rc = dynamic_cast<RenderComponent*>(*it);
        if(rc == 0) continue;
        
        if(rc->applyLighting == false) continue;
        
        RecursiveRender(rc, view, matrix4(1.0));
    }
    
    renderSystem->SetDrawBuffer(DRAW_BUFFER_BACK);
    
    renderSystem->DisableDepthTest();
    renderSystem->DisableFaceCulling();
    renderSystem->SetClearColor(vector4(0, 0, 0, 1));
    
    m_framebuffer->Unbind();
    m_program->Unbind();
}

void DepthPass::RecursiveRender(RenderComponent* rc, matrix4 view, matrix4 parent) {
    Transform* meshTransform = rc->transform;
    matrix4 mat = meshTransform->GetMatrix();
    matrix4 model = mat * parent;
    
    if(rc->children.size()) {
        auto it = rc->children.begin();
        for(; it != rc->children.end(); it++) {
            RecursiveRender((*it), view, model);
        }
        
        return;
    }
    
    // Send view/proj matrix to shader
    matrix4 modelviewMatrix = view * model;
    m_program->Set("modelviewMatrix", modelviewMatrix);
    m_program->Set("viewMatrix", view);
    m_program->Set("modelMatrix", model);
    
    Renderable* m = rc->renderable;
    VertexBuffer* vertexBuffer = m->vertexBuffer;
    VertexFormat* vertexType = vertexBuffer->GetFormat();
    int vertexCount = vertexBuffer->GetCount();
    
    vertexType->Bind();
    vertexBuffer->Bind();
    
    if(m->indexBuffer) {
        m->indexBuffer->Bind();
    }
    
    // Enable the attributes we need
    // Enable the attributes we need
    bool enabled = vertexType->EnableAttribute(0, VERTEXTYPE_ATTRIB_POSITION);
    m_program->Set("VERTEXTYPE_ATTRIB_POSITION", (int)enabled);
    enabled = vertexType->EnableAttribute(5, VERTEXTYPE_ATTRIB_BONES);
    m_program->Set("VERTEXTYPE_ATTRIB_BONES", (int)enabled);
    enabled = vertexType->EnableAttribute(6, VERTEXTYPE_ATTRIB_BONEWEIGHTS);
    m_program->Set("VERTEXTYPE_ATTRIB_BONEWEIGHTS", (int)enabled);
    
    // Get render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Draw
    if(m->indexBuffer) {
        int indexCount = m->indexBuffer->GetIndexCount();
        renderSystem->DrawIndexed(DRAW_TRIANGLES, indexCount);
    }
    else {
        renderSystem->Draw(DRAW_TRIANGLES, vertexCount);
    }
    
    vertexBuffer->Unbind();
    vertexType->Unbind();
}
