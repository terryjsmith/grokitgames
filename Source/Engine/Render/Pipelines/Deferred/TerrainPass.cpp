
#include <Render/Pipelines/Deferred/TerrainPass.h>
#include <Render/RenderSystem.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <Render/MaterialSystem.h>
#include <Render/TerrainComponent.h>

void TerrainPass::Initialize(int width, int height) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Framebuffer* fb = renderSystem->CreateFramebuffer();
    fb->Initialize();
    
    m_framebuffers.push_back(fb);
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("gbuffer.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("terrain.fs", "Shader"));
    
    // Create a program
    if (m_program == 0) {
        m_program = renderSystem->CreateShaderProgram();
        m_program->Initialize(vshader, fshader);
    }
}

void TerrainPass::SetNormalTexture(Texture2D* texture) {
    m_framebuffers[0]->AddTexture(texture, FRAMEBUFFER_SLOT_2);
}

void TerrainPass::SetPositionTexture(Texture2D* texture) {
    m_framebuffers[0]->AddTexture(texture, FRAMEBUFFER_SLOT_1);
}

void TerrainPass::SetDiffuseTexture(Texture2D* texture) {
    m_framebuffers[0]->AddTexture(texture, FRAMEBUFFER_SLOT_0);
}

void TerrainPass::SetDepthTexture(Texture2D* texture) {
    m_framebuffers[0]->AddTexture(texture, FRAMEBUFFER_SLOT_DEPTH);
}

void TerrainPass::Render(Scene* scene) {
    // Use shader program
    m_program->Bind();
    
    // Bind output framebuffer
    m_framebuffers[0]->Bind();
    
    // Get render system, set depth to less or equal
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->EnableDepthTest(TEST_LEQUAL);
    
    // DO NOT Clear our buffer - keep gbuffer pass
    // renderSystem->Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);
    
    // Get the camera
    CameraComponent* camera = scene->camera;
    
    // Get texture width/height
    int windowWidth = m_framebuffers[0]->GetTexture(0)->GetWidth();
    int windowHeight = m_framebuffers[0]->GetTexture(0)->GetHeight();
    
    camera->SetAspectRatio((float)windowWidth / windowHeight);
    
    // Get matrices
    matrix4 view = camera->GetViewMatrix();
    matrix4 proj = camera->GetProjectionMatrix();
    
    m_program->Set("projectionMatrix", proj);
    
    // Iterate over renderables
    auto it = scene->renderables.begin();
    for(; it != scene->renderables.end(); it++) {
        TerrainComponent* mc = dynamic_cast<TerrainComponent*>(*it);
        if(mc == 0) continue;
        
        matrix4 parent = glm::translate(matrix4(1.0f), vector3(mc->transform->GetWorldPosition()));
        TerrainQuad* quad = dynamic_cast<TerrainQuad*>(mc->renderable);
        RecursiveRender(quad, view, parent, mc);
    }
    
    //m_framebuffers[0]->GetTexture(0)->Save("diffuse.bmp");
    
    renderSystem->DisableDepthTest();
    
    m_framebuffers[0]->Unbind();
    m_program->Unbind();
}

void TerrainPass::RecursiveRender(TerrainQuad* rc, matrix4 view, matrix4 parent, TerrainComponent* tc) {
    // Calculate model matrix
    matrix4 mat = glm::translate(matrix4(1.0f), rc->GetOffset());
    matrix4 model = mat * parent;
    
    if(rc->children.size() > 0) {
        auto it = rc->children.begin();
        for(; it != rc->children.end(); it++) {
            TerrainQuad* quad = dynamic_cast<TerrainQuad*>(*it);
            RecursiveRender(quad, view, model, tc);
        }
        
        return;
    }
    
    if(rc->GetLOD() == -1) {
        return;
    }
    
    Renderable* m = rc;
    
    // Send view/proj matrix to shader
    matrix4 modelviewMatrix = view * model;
    m_program->Set("modelviewMatrix", modelviewMatrix);

    VertexBuffer* vertexBuffer = m->vertexBuffer;
    VertexFormat* vertexType = vertexBuffer->GetFormat();
    int vertexCount = vertexBuffer->GetCount();
    
    vertexType->Bind();
    vertexBuffer->Bind();
    
    if(m->indexBuffer) {
        m->indexBuffer->Bind();
    }
    
    // Enable the attributes we need
    bool enabled = vertexType->EnableAttribute(0, VERTEXTYPE_ATTRIB_POSITION);
    m_program->Set("VERTEXTYPE_ATTRIB_POSITION", (int)enabled);
    enabled = vertexType->EnableAttribute(1, VERTEXTYPE_ATTRIB_COLOR);
    m_program->Set("VERTEXTYPE_ATTRIB_COLOR", (int)enabled);
    enabled = vertexType->EnableAttribute(2, VERTEXTYPE_ATTRIB_NORMAL);
    m_program->Set("VERTEXTYPE_ATTRIB_NORMAL", (int)enabled);
    enabled = vertexType->EnableAttribute(3, VERTEXTYPE_ATTRIB_TEXCOORD0);
    m_program->Set("VERTEXTYPE_ATTRIB_TEXCOORD0", (int)enabled);
    enabled = vertexType->EnableAttribute(4, VERTEXTYPE_ATTRIB_TEXCOORD1);
    m_program->Set("VERTEXTYPE_ATTRIB_TEXCOORD1", (int)enabled);

    // Bind number of textures
    int numTextures = tc->GetNumTextures();
    m_program->Set("numTextures", numTextures);
    
    // Bind textures
    for(int i = 0; i < numTextures; i++) {
        tc->GetTexture(i)->Bind(i);
        m_program->Set("diffuseTextures[" + std::to_string(i) + "]", i);
        
        tc->GetTexture(i)->SetAnisotropicFilter(16.0f);
        tc->GetTexture(i)->SetWrapping(true);
    }
    
    if(m->normalTexture) {
        m->normalTexture->Bind(5);
        m_program->Set("normalTexture", 5);
    }
    
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
    
    for(int i = 0; i < numTextures; i++) {
        tc->GetTexture(i)->Unbind();
    }
    
    if(m->normalTexture) {
        m->normalTexture->Unbind();
    }
    
    vertexBuffer->Unbind();
    vertexType->Unbind();
}
