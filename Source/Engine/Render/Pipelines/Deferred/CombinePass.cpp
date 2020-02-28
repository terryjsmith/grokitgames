
#include <Render/Pipelines/Deferred/CombinePass.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>

CombinePass::CombinePass() {
    m_windowWidth = m_windowHeight = 0;
    m_vertexBuffer = 0;
    m_vertexFormat = 0;
    m_diffuseTexture = 0;
    m_lightingTexture = 0;
}

void CombinePass::Initialize(int width, int height) {
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
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("combine.fs", "Shader"));
    
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
}

void CombinePass::Render(Scene* scene) {
    // Use shader program
    m_program->Bind();
    
    // Get render system, disable depth test
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->DisableDepthTest();
    
    m_framebuffers[0]->Bind();
    
    // Clear our buffer
    renderSystem->Clear(COLOR_BUFFER_BIT);
    
    // Get matrices
    matrix4 proj = glm::ortho(0.0f, (float)m_windowWidth, (float)m_windowHeight, 0.0f);
    
    m_program->Set("ortho", proj);
    
    int vertexCount = m_vertexBuffer->GetCount();
    
    m_vertexFormat->Bind();
    m_vertexBuffer->Bind();
    
    // Enable the attributes we need
    m_vertexFormat->EnableAttribute(0, VERTEXTYPE_ATTRIB_POSITION);
    m_vertexFormat->EnableAttribute(3, VERTEXTYPE_ATTRIB_TEXCOORD0);
    
    // Bind textures
    m_diffuseTexture->Bind(0);
    m_program->Set("textureDiffuse", 0);
    
    m_lightingTexture->Bind(1);
    m_program->Set("textureLighting", 1);
    
    // Ambient lighting
    vector3 ambient = renderSystem->GetAmbientLighting();
    m_program->Set("ambientLight", ambient);

    renderSystem->Draw(DRAW_TRIANGLE_STRIP, vertexCount);
    
    m_diffuseTexture->Unbind();
    m_lightingTexture->Unbind();
    m_program->Unbind();
    
    m_vertexBuffer->Unbind();
    m_vertexFormat->Unbind();
    
    //m_framebuffers[0]->GetTexture(0)->Save("combine.bmp");
}
