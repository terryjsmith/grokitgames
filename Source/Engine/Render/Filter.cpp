
#include <Render/Filter.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>
#include <Render/Defines.h>
#include <IO/ResourceSystem.h>

Filter::Filter() {
    m_inputTexture = 0;
    m_outputTexture = 0;
    m_framebuffer = 0;;
    m_program = 0;
    m_width = m_height = 0;
    m_vertexBuffer = 0;
    m_vertexFormat = 0;
}

Filter::~Filter() {
    if(m_outputTexture) {
        delete m_outputTexture;
        m_outputTexture = 0;
    }
    
    if(m_framebuffer) {
        delete m_framebuffer;
        m_framebuffer = 0;
    }
    
    if(m_vertexBuffer) {
        delete m_vertexBuffer;
        m_vertexBuffer = 0;
    }
    
    if(m_vertexFormat) {
        delete m_vertexFormat;
        m_vertexFormat = 0;
    }
}

void Filter::Initialize(std::string shader, int width, int height) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    m_framebuffer = renderSystem->CreateFramebuffer();
    m_framebuffer->Initialize();
    
    m_outputTexture = renderSystem->CreateTexture2D();
    m_outputTexture->Initialize(width, height, COLOR_RGB16F, TEXTURE_TYPE_FLOAT, COLOR_RGB);
    
    m_framebuffer->AddTexture(m_outputTexture, FRAMEBUFFER_SLOT_0);
    
    // Load shaders
    Shader* vshader = dynamic_cast<Shader*>(resourceSystem->LoadResource("ortho.vs", "Shader"));
    Shader* fshader = dynamic_cast<Shader*>(resourceSystem->LoadResource(shader, "Shader"));
    
    // Create a program
    if (m_program == 0) {
        m_program = renderSystem->CreateShaderProgram();
        m_program->Initialize(vshader, fshader);
    }
    
    m_width = width;
    m_height = height;
    
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

