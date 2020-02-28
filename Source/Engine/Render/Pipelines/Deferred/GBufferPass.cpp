
#include <Render/Pipelines/Deferred/GBufferPass.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>
#include <Render/MaterialSystem.h>

void GBufferPass::Initialize(int width, int height) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Framebuffer* fb = renderSystem->CreateFramebuffer();
    fb->Initialize();
    
    Texture2D* diffuse = renderSystem->CreateTexture2D();
    diffuse->Initialize(width, height, COLOR_RGB16F, COLOR_RGB);
    fb->AddTexture(diffuse, FRAMEBUFFER_SLOT_0);
    
    Texture2D* position = renderSystem->CreateTexture2D();
    position->Initialize(width, height, COLOR_RGB16F, COLOR_RGB);
    fb->AddTexture(position, FRAMEBUFFER_SLOT_1);
    
    Texture2D* normal = renderSystem->CreateTexture2D();
    normal->Initialize(width, height, COLOR_RGB16F, COLOR_RGB);
    fb->AddTexture(normal, FRAMEBUFFER_SLOT_2);
    
    Texture2D* aux = renderSystem->CreateTexture2D();
    aux->Initialize(width, height, COLOR_RGB16F, COLOR_RGB);
    fb->AddTexture(aux, FRAMEBUFFER_SLOT_3);
    
    Texture2D* depth = renderSystem->CreateTexture2D();
    depth->Initialize(width, height, COLOR_DEPTH_COMPONENT24, COLOR_DEPTH_COMPONENT);
    fb->AddTexture(depth, FRAMEBUFFER_SLOT_DEPTH);
    
    m_framebuffers.push_back(fb);
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("gbuffer.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("gbuffer.fs", "Shader"));
    
    // Create a program
    if (m_program == 0) {
        m_program = renderSystem->CreateShaderProgram();
        m_program->Initialize(vshader, fshader);
    }
}

void GBufferPass::Render(Scene* scene) {
    // Use shader program
    m_program->Bind();
    
    // Bind output framebuffer
    m_framebuffers[0]->Bind();
    
    // Get render system, set depth to less or equal
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->EnableDepthTest(TEST_LEQUAL);
    
    // Clear our buffer
    renderSystem->Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);
    
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
        MeshComponent* mc = dynamic_cast<MeshComponent*>(*it);
        if(mc == 0) continue;
        
        if(mc->applyLighting == false) continue;
        
        RecursiveRender(mc, view, matrix4(1.0));
    }
    
    //m_framebuffers[0]->GetTexture(0)->Save("diffuse.bmp");
    
    renderSystem->DisableDepthTest();
    
    m_framebuffers[0]->Unbind();
    m_program->Unbind();
}

void GBufferPass::RecursiveRender(MeshComponent* rc, matrix4 view, matrix4 parent) {
    // Calculate model matrix
    Transform* meshTransform = rc->transform;
    matrix4 mat = meshTransform->GetMatrix();
    matrix4 model = mat * parent;
    
    if(rc->children.size() > 0) {
        auto it = rc->children.begin();
        for(; it != rc->children.end(); it++) {
            RecursiveRender((*it), view, model);
        }
        
        return;
    }
    
    Renderable* m = rc->renderable;
    
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
    
    // Get our material texture
    MaterialSystem* materialSystem = GetSystem<MaterialSystem>();
    Texture* materialTexture = materialSystem->GetTexture();
    
    materialTexture->Bind(3);
    m_program->Set("textureMaterialLookup", 3);
    materialTexture->SetTextureFilter(FILTER_NEAREST);
    
    // Specify material
    m_program->Set("materialID", (float)m->material->GetMaterial());
    
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
