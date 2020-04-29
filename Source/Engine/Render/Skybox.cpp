
#include <Render/Skybox.h>
#include <Core/Application.h>
#include <IO/ResourceSystem.h>
#include <Render/RenderSystem.h>
#include <Render/Defines.h>

void Skybox::Initialize(std::string prefix) {
    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    // Create cube map
    cubeMap = renderSystem->CreateTexture3D();
    
    // Load textures
    cubeMap->Load(CUBEMAP_FACE_TOP, prefix + "_top.png");
    cubeMap->Load(CUBEMAP_FACE_BOTTOM, prefix + "_bottom.png");
    cubeMap->Load(CUBEMAP_FACE_LEFT, prefix + "_left.png");
    cubeMap->Load(CUBEMAP_FACE_RIGHT, prefix + "_right.png");
    cubeMap->Load(CUBEMAP_FACE_FRONT, prefix + "_front.png");
    cubeMap->Load(CUBEMAP_FACE_BACK, prefix + "_back.png");
    
    //cubeMap->Save("cubemap.bmp");
    
    // Initialize renderable
    renderable = new Renderable();
    renderable->vertexBuffer = renderSystem->CreateVertexBuffer();
    
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    VertexFormat* fmt = renderSystem->CreateVertexFormat();
    fmt->Initialize();
    fmt->Bind();
    
    fmt->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 3, 0);
    
    renderable->vertexBuffer->Create(fmt, 36, skyboxVertices, false);
}
