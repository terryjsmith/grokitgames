
#include <Render/GaussianBlur.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>

GaussianBlur::GaussianBlur() {
    m_tempTexture = 0;
}

GaussianBlur::~GaussianBlur() {
    if(m_tempTexture) {
        delete m_tempTexture;
        m_tempTexture = 0;
    }
}

void GaussianBlur::Apply() {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    if(m_tempTexture == 0) {
        m_tempTexture = renderSystem->CreateTexture2D();
        m_tempTexture->Initialize(m_width, m_height, COLOR_RGB16F, COLOR_RGB);
    }
    
    // Use shader program
    m_program->Bind();
    
    // Get render system, disable depth test
    renderSystem->DisableDepthTest();
    
    m_framebuffer->SetTexture(m_tempTexture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
    m_framebuffer->Bind();
    
    // Clear our buffer
    renderSystem->Clear(COLOR_BUFFER_BIT);
    
    // Get matrices
    int width = m_framebuffer->GetTexture(0)->GetWidth();
    int height = m_framebuffer->GetTexture(0)->GetHeight();
    
    matrix4 proj = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
    
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
    m_program->Set("samples", (int)21);
    
    m_program->Set("horizontal", 0);
    renderSystem->Draw(DRAW_TRIANGLE_STRIP, vertexCount);
    
    // Flip to second pass
    m_tempTexture->Bind(0);
    m_program->Set("inputTexture", 0);
    
    m_framebuffer->SetTexture(m_outputTexture, COLOR_RGB16F, FRAMEBUFFER_SLOT_0);
    m_framebuffer->Bind();
    
    m_program->Set("horizontal", 1);
    renderSystem->Draw(DRAW_TRIANGLE_STRIP, vertexCount);
    
    m_inputTexture->Unbind();
    m_program->Unbind();
    
    m_vertexBuffer->Unbind();
    m_vertexFormat->Unbind();
}
