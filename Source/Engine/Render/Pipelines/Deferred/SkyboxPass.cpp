
#include <Render/Pipelines/Deferred/SkyboxPass.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>
#include <Render/Skybox.h>

SkyboxPass::~SkyboxPass() {
    // Reset
    if(m_texture) {
        // Add our texture back to vertex buffer to ensure it is deleted, not the one currently attached
        m_framebuffers[0]->SetTexture(m_texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
    }

    auto fi = m_framebuffers.begin();
    for(; fi != m_framebuffers.end(); fi++) {
        delete(*fi);
    }
    m_framebuffers.clear();
}

void SkyboxPass::Initialize(int width, int height) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Framebuffer* fb = renderSystem->CreateFramebuffer();
    fb->Initialize();
    
    // Reset
    if(m_texture) {
        // Add our texture back to vertex buffer to ensure it is deleted, not the one currently attached
        m_framebuffers[0]->SetTexture(m_texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
    }

    auto fi = m_framebuffers.begin();
    for(; fi != m_framebuffers.end(); fi++) {
        delete(*fi);
    }
    m_framebuffers.clear();
    
    m_texture = renderSystem->CreateTexture2D();
    m_texture->Initialize(width, height, COLOR_RGB16F, TEXTURE_TYPE_FLOAT, COLOR_RGB);
    
    fb->AddTexture(m_texture, FRAMEBUFFER_SLOT_0);
    
    m_framebuffers.push_back(fb);
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("skybox.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("skybox.fs", "Shader"));
    
    // Create a program
    if (m_program == 0) {
        m_program = renderSystem->CreateShaderProgram();
        m_program->Initialize(vshader, fshader);
    }
}

void SkyboxPass::SetInputTexture(Texture2D* texture) {
    m_framebuffers[0]->SetTexture(texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
}

void SkyboxPass::SetDepthTexture(Texture2D* texture) {
    m_framebuffers[0]->SetTexture(texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_DEPTH);
}

void SkyboxPass::Render(Scene* scene) {
    m_program->Bind();
    
    // Bind output framebuffer
    m_framebuffers[0]->Bind();

    // Get render system, set depth to less or equal
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->EnableDepthTest(TEST_LEQUAL);
    
    // Get the camera
    CameraComponent* camera = scene->camera;
    
    // Get texture width/height
    int windowWidth = m_framebuffers[0]->GetTexture(0)->GetWidth();
    int windowHeight = m_framebuffers[0]->GetTexture(0)->GetHeight();
    
    camera->SetAspectRatio((float)windowWidth / windowHeight);
    
    // Get matrices
    matrix4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
    matrix4 proj = camera->GetProjectionMatrix();
    
    m_program->Set("projectionMatrix", proj);
    
    // Iterate over renderables
    auto it = scene->renderables.begin();
    for(; it != scene->renderables.end(); it++) {
        Skybox* mc = dynamic_cast<Skybox*>(*it);
        if(mc == 0) continue;

        // Render skybox
        
        // Calculate model matrix
        Transform* meshTransform = mc->transform;
        matrix4 model = meshTransform->GetMatrix();
        
        // Send view/proj matrix to shader
        matrix4 modelviewMatrix = view * model;
        m_program->Set("modelviewMatrix", modelviewMatrix);
        
        Renderable* m = mc->renderable;
        VertexBuffer* vertexBuffer = m->vertexBuffer;
        VertexFormat* vertexType = vertexBuffer->GetFormat();
        int vertexCount = vertexBuffer->GetCount();
        
        vertexType->Bind();
        vertexBuffer->Bind();
        
        // Enable the attributes we need
        vertexType->EnableAttribute(0, VERTEXTYPE_ATTRIB_POSITION);
        
        mc->cubeMap->Bind(0);
        m_program->Set("skybox", 0);
        
        renderSystem->Draw(DRAW_TRIANGLES, vertexCount);
        
        mc->cubeMap->Unbind();
        vertexBuffer->Unbind();
        vertexType->Unbind();
    }
    
    renderSystem->DisableDepthTest();
    
    m_framebuffers[0]->Unbind();
    m_program->Unbind();
}
/*
void ForwardPass::RecursiveRender(MeshComponent* rc, matrix4 view, matrix4 parent) {
    // Calculate model matrix
    Transform* meshTransform = rc->transform;
    matrix4 mat = meshTransform->GetMatrix();
    matrix4 model = mat * parent;
    
    if(rc->children.size()) {
        auto it = rc->children.begin();
        for(; it != rc->children.end(); it++) {
            MeshComponent* mc = dynamic_cast<MeshComponent*>(*it);
            if(mc == 0) continue;
            
            RecursiveRender(mc, view, model);
        }
        
        return;
    }
    
    // Send view/proj matrix to shader
    matrix4 modelviewMatrix = view * model;
    m_program->Set("modelviewMatrix", modelviewMatrix);
    
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
    bool enabled = vertexType->EnableAttribute(0, VERTEXTYPE_ATTRIB_POSITION);
    m_program->Set("VERTEXTYPE_ATTRIB_POSITION", (int)enabled);
    enabled = vertexType->EnableAttribute(1, VERTEXTYPE_ATTRIB_COLOR);
    m_program->Set("VERTEXTYPE_ATTRIB_COLOR", (int)enabled);
    enabled = vertexType->EnableAttribute(3, VERTEXTYPE_ATTRIB_TEXCOORD0);
    m_program->Set("VERTEXTYPE_ATTRIB_TEXCOORD0", (int)enabled);
    enabled = vertexType->EnableAttribute(4, VERTEXTYPE_ATTRIB_TEXCOORD1);
    m_program->Set("VERTEXTYPE_ATTRIB_TEXCOORD1", (int)enabled);
    enabled = vertexType->EnableAttribute(5, VERTEXTYPE_ATTRIB_BONES);
    m_program->Set("VERTEXTYPE_ATTRIB_BONES", (int)enabled);
    enabled = vertexType->EnableAttribute(6, VERTEXTYPE_ATTRIB_BONEWEIGHTS);
    m_program->Set("VERTEXTYPE_ATTRIB_BONEWEIGHTS", (int)enabled);
    
    // Bind textures
    if(m->diffuseTexture) {
        m->diffuseTexture->Bind(0);
        m_program->Set("diffuseTexture", 0);
        
        // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
    }
    
    if(m->normalTexture) {
        m->normalTexture->Bind(1);
        m_program->Set("normalTexture", 1);
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
    
    if(m->diffuseTexture) {
        m->diffuseTexture->Unbind();
    }
    
    if(m->normalTexture) {
        m->normalTexture->Unbind();
    }
    
    vertexBuffer->Unbind();
    vertexType->Unbind();
}
*/
