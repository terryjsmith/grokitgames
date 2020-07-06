
#include <Render/Pipelines/Deferred/ForwardPass.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>

ForwardPass::ForwardPass() {
    m_texture = 0;
}

ForwardPass::~ForwardPass() {
    // Reset
    if(m_texture) {
        // Add our texture back to vertex buffer to ensure it is deleted, not the one currently attached
        m_framebuffers[0]->SetTexture(m_texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
        m_framebuffers[0]->SetTexture(m_texture2, COLOR_RGB16F, FRAMEBUFFER_SLOT_1);
    }
    
    auto fi = m_framebuffers.begin();
    for(; fi != m_framebuffers.end(); fi++) {
        delete(*fi);
    }
    m_framebuffers.clear();
}

void ForwardPass::Initialize(int width, int height) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    Framebuffer* fb = renderSystem->CreateFramebuffer();
    fb->Initialize();
    
    // Reset
    if(m_texture) {
        // Add our texture back to vertex buffer to ensure it is deleted, not the one currently attached
        m_framebuffers[0]->SetTexture(m_texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
        m_framebuffers[0]->SetTexture(m_texture2, COLOR_RGB16F, FRAMEBUFFER_SLOT_1);
    }
    
    auto fi = m_framebuffers.begin();
    for(; fi != m_framebuffers.end(); fi++) {
        delete(*fi);
    }
    m_framebuffers.clear();
    
    m_texture = renderSystem->CreateTexture2D();
    m_texture->Initialize(width, height, COLOR_RGB16F, TEXTURE_TYPE_FLOAT, COLOR_RGB);
    
    m_texture2 = renderSystem->CreateTexture2D();
    m_texture2->Initialize(width, height, COLOR_RGB16F, TEXTURE_TYPE_FLOAT, COLOR_RGB);
    
    fb->AddTexture(m_texture, FRAMEBUFFER_SLOT_0);
    fb->AddTexture(m_texture2, FRAMEBUFFER_SLOT_1);
    
    m_framebuffers.push_back(fb);
    
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("forward.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("forward.fs", "Shader"));
    
    // Create a program
    if (m_program == 0) {
        m_program = renderSystem->CreateShaderProgram();
        m_program->Initialize(vshader, fshader);
    }
}

void ForwardPass::SetInputTexture(Texture2D* texture) {
    m_framebuffers[0]->SetTexture(texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
}

void ForwardPass::SetAuxTexture(Texture2D* texture) {
    m_framebuffers[0]->SetTexture(texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_1);
}

void ForwardPass::SetDepthTexture(Texture2D* texture) {
    m_framebuffers[0]->SetTexture(texture, COLOR_RGB16F, FRAMEBUFFER_SLOT_DEPTH);
}

void ForwardPass::Render(Scene* scene) {
    m_program->Bind();
    
    // Bind output framebuffer
    m_framebuffers[0]->Bind();

    // Get render system, set depth to less or equal
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->EnableDepthTest(TEST_LEQUAL);
    
    renderSystem->EnableBlending();
    renderSystem->SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    
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
    for(int i = 0; i < scene->renderables.size(); i++) {
        MeshComponent* mc = dynamic_cast<MeshComponent*>(scene->renderables[i]);
        ShaderProgram* program = m_program;
        if(mc == 0) continue;
        
        if(mc->applyLighting == true) continue;
        
        if(mc->program) {
            mc->program->Set("projectionMatrix", proj);
            program = mc->program;
        }
        
        program->Set("sceneIndex", (float)(i+1));
        
        mc->PreRender(scene);
        RecursiveRender(mc, view, matrix4(1.0), program);
        
        if(mc->program) {
            mc->program->Unbind();
            m_program->Bind();
        }
    }

    renderSystem->DisableBlending();
    renderSystem->DisableDepthTest();
    
    m_framebuffers[0]->Unbind();
    m_program->Unbind();
}

void ForwardPass::RecursiveRender(MeshComponent* rc, matrix4 view, matrix4 parent, ShaderProgram* program) {
    // Calculate model matrix
    Transform* meshTransform = rc->GetTransform();
    matrix4 mat = meshTransform->GetMatrix();
    matrix4 model = mat * parent;
    
    if(rc->children.size()) {
        auto it = rc->children.begin();
        for(; it != rc->children.end(); it++) {
            MeshComponent* mc = dynamic_cast<MeshComponent*>(*it);
            if(mc == 0) continue;
            
            RecursiveRender(mc, view, model, program);
        }
        
        return;
    }
    
    // Send view/proj matrix to shader
    matrix4 modelviewMatrix = view * model;
    program->Set("modelviewMatrix", modelviewMatrix);
    
    Renderable* m = rc->renderable;
    if(m == 0) return;

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
    program->Set("VERTEXTYPE_ATTRIB_POSITION", (int)enabled);
    enabled = vertexType->EnableAttribute(1, VERTEXTYPE_ATTRIB_COLOR);
    program->Set("VERTEXTYPE_ATTRIB_COLOR", (int)enabled);
    enabled = vertexType->EnableAttribute(3, VERTEXTYPE_ATTRIB_TEXCOORD0);
    program->Set("VERTEXTYPE_ATTRIB_TEXCOORD0", (int)enabled);
    enabled = vertexType->EnableAttribute(4, VERTEXTYPE_ATTRIB_TEXCOORD1);
    program->Set("VERTEXTYPE_ATTRIB_TEXCOORD1", (int)enabled);
    enabled = vertexType->EnableAttribute(5, VERTEXTYPE_ATTRIB_BONES);
    program->Set("VERTEXTYPE_ATTRIB_BONES", (int)enabled);
    enabled = vertexType->EnableAttribute(6, VERTEXTYPE_ATTRIB_BONEWEIGHTS);
    program->Set("VERTEXTYPE_ATTRIB_BONEWEIGHTS", (int)enabled);
    enabled = vertexType->EnableAttribute(7, VERTEXTYPE_ATTRIB_OFFSETS);
    program->Set("VERTEXTYPE_ATTRIB_OFFSETS", (int)enabled);
    
    // Bind textures
    if(m->diffuseTexture) {
        m->diffuseTexture->Bind(0);
        program->Set("diffuseTexture", 0);
        
        // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
    }
    
    if(m->normalTexture) {
        m->normalTexture->Bind(1);
        program->Set("normalTexture", 1);
    }
    
    // Get render system
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Draw
    if(rc->instanced == true) {
        if(m->indexBuffer) {
            int indexCount = m->indexBuffer->GetIndexCount();
            renderSystem->DrawInstancedIndexed(DRAW_TRIANGLES, indexCount, rc->instances);
        }
        else {
            renderSystem->DrawInstanced(DRAW_TRIANGLES, vertexCount, rc->instances);
        }
    }
    else {
        if(m->indexBuffer) {
            int indexCount = m->indexBuffer->GetIndexCount();
            renderSystem->DrawIndexed(DRAW_TRIANGLES, indexCount);
        }
        else {
            renderSystem->Draw(DRAW_TRIANGLES, vertexCount);
        }
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
