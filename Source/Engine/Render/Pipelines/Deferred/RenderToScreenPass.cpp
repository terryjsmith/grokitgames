
#include <Render/Pipelines/Deferred/RenderToScreenPass.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>

RenderToScreenPass::RenderToScreenPass() {
    m_windowWidth = m_windowHeight = 0;
    m_vertexBuffer = 0;
    m_vertexFormat = 0;
    m_inputTexture = 0;
}

void RenderToScreenPass::Initialize(int width, int height) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("ortho.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("ortho.fs", "Shader"));
    
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

void RenderToScreenPass::Render(Scene* scene) {
    // Use shader program
    m_program->Bind();
    
    // Get render system, disable depth test
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    renderSystem->DisableDepthTest();
    
    renderSystem->UseDefaultFramebuffer();
    renderSystem->SetViewport(m_windowWidth, m_windowHeight);
    
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
    m_inputTexture->Bind(0);
    m_program->Set("inputTexture", 0);

    renderSystem->Draw(DRAW_TRIANGLE_STRIP, vertexCount);
    
    m_inputTexture->Unbind();
    m_program->Unbind();
    
    m_vertexBuffer->Unbind();
    m_vertexFormat->Unbind();
}
