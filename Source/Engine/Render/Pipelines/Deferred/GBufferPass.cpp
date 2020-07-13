
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
    
    // Reset
    auto fi = m_framebuffers.begin();
    for(; fi != m_framebuffers.end(); fi++) {
        delete(*fi);
    }
    m_framebuffers.clear();
    
    Texture2D* diffuse = renderSystem->CreateTexture2D();
    diffuse->Initialize(width, height, COLOR_RGB16F, TEXTURE_TYPE_FLOAT, COLOR_RGB);
    fb->AddTexture(diffuse, FRAMEBUFFER_SLOT_0);
    
    Texture2D* position = renderSystem->CreateTexture2D();
    position->Initialize(width, height, COLOR_RGB16F, TEXTURE_TYPE_FLOAT, COLOR_RGB);
    fb->AddTexture(position, FRAMEBUFFER_SLOT_1);
    
    Texture2D* normal = renderSystem->CreateTexture2D();
    normal->Initialize(width, height, COLOR_RGB16F, TEXTURE_TYPE_FLOAT, COLOR_RGB);
    fb->AddTexture(normal, FRAMEBUFFER_SLOT_2);
    
    Texture2D* aux = renderSystem->CreateTexture2D();
    aux->Initialize(width, height, COLOR_RGBA, TEXTURE_TYPE_FLOAT, COLOR_RGBA);
    fb->AddTexture(aux, FRAMEBUFFER_SLOT_3);
    
    Texture2D* depth = renderSystem->CreateTexture2D();
    depth->Initialize(width, height, COLOR_DEPTH_COMPONENT24, TEXTURE_TYPE_FLOAT, COLOR_DEPTH_COMPONENT);
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
    m_program->Set("cameraPosition", camera->GetTransform()->GetWorldPosition());
    
    // Iterate over renderables
    for(int i = 0 ; i < scene->renderables.size(); i++) {
        MeshComponent* mc = dynamic_cast<MeshComponent*>(scene->renderables[i]);
        ShaderProgram* program = m_program;
        if(mc == 0) continue;
        
        if(mc->applyLighting == false) continue;
        
        if(mc->program) {
            mc->program->Bind();
            mc->program->Set("projectionMatrix", proj);
            mc->program->Set("cameraPosition", camera->GetTransform()->GetWorldPosition());
            
            program = mc->program;
        }
        
        program->Set("sceneIndex", (float)(i+1));
        
        // Load bones
        if(mc->animation) {
            auto bi = mc->animation->bones.begin();
            for(; bi != mc->animation->bones.end(); bi++) {
                int index = std::distance(mc->animation->bones.begin(), bi);
                program->Set("boneMatrix[" + std::to_string(index) + "]", bi->second);
            }
        }
        
        mc->PreRender(scene);
        RecursiveRender(mc, view, matrix4(1.0), scene, program);
        
        if(mc->program) {
            mc->program->Unbind();
            m_program->Bind();
        }
    }
    
    renderSystem->DisableDepthTest();
    
    m_framebuffers[0]->Unbind();
    m_program->Unbind();
}

void GBufferPass::RecursiveRender(MeshComponent* rc, matrix4 view, matrix4 parent, Scene* scene, ShaderProgram* program) {
    // Calculate model matrix
    Transform* meshTransform = rc->GetTransform();
    matrix4 mat = meshTransform->GetMatrix();
    matrix4 model = mat * parent;
    
    int counter = 0;
    if(rc->children.size() > 0) {
        auto it = rc->children.begin();
        for(; it != rc->children.end(); it++) {
            counter++;
            MeshComponent* mc = dynamic_cast<MeshComponent*>(*it);
            if(mc == 0) {
                continue;
            }
            
            program->Set("childIndex", (float)counter);
            RecursiveRender(mc, view, model, scene, program);
        }
        
        return;
    }
    
    Renderable* m = rc->renderable;
    if(m == 0) return;
    
    CameraComponent* cc = scene->camera;
    Frustum frustum = cc->GetFrustum();
    vector3 cameraPosition = cc->GetTransform()->GetWorldPosition();
    Sphere* boundingSphere = rc->GetBoundingSphere(parent);
    
    if(frustum.Intersects(boundingSphere) == false && boundingSphere->Inside(cameraPosition) == false) {
        return;
    }
    
    // Send view/proj matrix to shader
    matrix4 modelviewMatrix = view * model;
    program->Set("modelviewMatrix", modelviewMatrix);
    program->Set("modelMatrix", model);
    
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
    enabled = vertexType->EnableAttribute(2, VERTEXTYPE_ATTRIB_NORMAL);
    program->Set("VERTEXTYPE_ATTRIB_NORMAL", (int)enabled);
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
    
    // Get our material texture
    MaterialSystem* materialSystem = GetSystem<MaterialSystem>();
    Texture* materialTexture = materialSystem->GetTexture();
    
    materialTexture->Bind(3);
    program->Set("textureMaterialLookup", 3);
    materialTexture->SetTextureFilter(FILTER_NEAREST);
    
    // Specify material
    int materialID = 0;
    if(m->material) {
        materialID = m->material->GetMaterial();
    }
    program->Set("materialID", (float)materialID);
    
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
