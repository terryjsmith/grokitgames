
#include <Render/Pipelines/Deferred/TerrainPass.h>
#include <Render/RenderSystem.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <Render/MaterialSystem.h>
#include <Render/TerrainComponent.h>

TerrainPass::~TerrainPass() {
    if(m_framebuffers.size()) {
        auto fi = m_framebuffers.begin();
        for(; fi != m_framebuffers.end(); fi++) {
            (*fi)->ClearTextures();
            delete(*fi);
        }
    }
    m_framebuffers.clear();
}

void TerrainPass::Initialize(int width, int height) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Framebuffer* fb = renderSystem->CreateFramebuffer();
    fb->Initialize();
    
    // Reset
    if(m_framebuffers.size()) {
        auto fi = m_framebuffers.begin();
        for(; fi != m_framebuffers.end(); fi++) {
            (*fi)->ClearTextures();
            delete(*fi);
        }
    }
    m_framebuffers.clear();
    
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

void TerrainPass::SetAuxTexture(Texture2D* texture) {
    m_framebuffers[0]->AddTexture(texture, FRAMEBUFFER_SLOT_3);
}

void TerrainPass::Render(Scene* scene) {
    // Use shader program
    m_program->Bind();
    
    // Bind output framebuffer
    m_framebuffers[0]->Bind();
    
    // Get render system, set depth to less or equal
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->EnableDepthTest(TEST_LEQUAL);
    renderSystem->EnableFaceCulling(CULLMODE_BACK);
    
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
    m_program->Set("cameraPosition", camera->GetTransform()->GetWorldPosition());
    
    // Iterate over renderables
    auto it = scene->renderables.begin();
    for(; it != scene->renderables.end(); it++) {
        TerrainComponent* mc = dynamic_cast<TerrainComponent*>(*it);
        if(mc == 0) continue;
        
        if(mc->GetSplatMap() != 0) {
            m_program->Set("splatMap", 0);
            mc->GetSplatMap()->Bind(0);
        }
        
        // Bind number of textures
        int numTextures = mc->GetNumTextures();
        m_program->Set("numTextures", numTextures);
        
        // Bind textures
        int baseTexture = 1;
        for(int i = 0; i < numTextures; i++) {
            mc->GetTexture(i)->Bind(baseTexture + i);
            m_program->Set("diffuseTextures[" + std::to_string(i) + "]", baseTexture + i);
            
            mc->GetTexture(i)->SetAnisotropicFilter(16.0f);
            mc->GetTexture(i)->SetWrapping(true);
        }
        baseTexture += numTextures;
        
        matrix4 parent = glm::translate(matrix4(1.0f), vector3(mc->GetTransform()->GetWorldPosition()));
        TerrainQuad* quad = dynamic_cast<TerrainQuad*>(mc->children[0]);
        RecursiveRender(quad, view, parent, scene);
        
        for(int i = 0; i < numTextures; i++) {
            mc->GetTexture(i)->Unbind();
        }
    }
    
    //m_framebuffers[0]->GetTexture(0)->Save("diffuse.bmp");
    
    renderSystem->DisableFaceCulling();
    renderSystem->DisableDepthTest();
    
    m_framebuffers[0]->Unbind();
    m_program->Unbind();
}

void TerrainPass::RecursiveRender(TerrainQuad* rc, matrix4 view, matrix4 parent, Scene* scene) {
    // Calculate model matrix
    matrix4 mat = glm::translate(matrix4(1.0f), rc->GetTransform()->GetLocalPosition());
    matrix4 model = mat * parent;
    
    if(rc->children.size() > 0) {
        auto it = rc->children.begin();
        for(; it != rc->children.end(); it++) {
            TerrainQuad* quad = dynamic_cast<TerrainQuad*>(*it);
            RecursiveRender(quad, view, model, scene);
        }
        
        return;
    }
    
    CameraComponent* cc = scene->camera;
    Frustum frustum = cc->GetFrustum();
    vector3 cameraPosition = cc->GetTransform()->GetWorldPosition();
    Sphere* boundingSphere = rc->GetBoundingSphere();
    
    if(frustum.Intersects(boundingSphere) == false && boundingSphere->Inside(cameraPosition) == false) {
        return;
    }
    
    if(rc->GetLOD() == -1) {
        return;
    }
    
    Renderable* m = rc->renderable;
    
    // Send view/proj matrix to shader
    matrix4 modelviewMatrix = view * model;
    m_program->Set("modelviewMatrix", modelviewMatrix);
    m_program->Set("modelMatrix", model);

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
    
    if(m->normalTexture) {
        m->normalTexture->Bind(0);
        m_program->Set("normalTexture", 0);
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
    
    if(m->normalTexture) {
        m->normalTexture->Unbind();
    }
    
    vertexBuffer->Unbind();
    vertexType->Unbind();
}
