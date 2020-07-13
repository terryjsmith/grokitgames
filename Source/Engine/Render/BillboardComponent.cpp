
#include <Render/BillboardComponent.h>
#include <Render/Scene.h>
#include <Render/RenderSystem.h>
#include <IO/ResourceSystem.h>
#include <Core/Application.h>

BillboardComponent::BillboardComponent() {
    m_size = 1.0f;
    m_texture = 0;
}

void BillboardComponent::Create(Texture2D* tex, float size) {
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    
    m_texture = tex;
    m_size = size;
    
    if(renderable == 0) {
        renderable = new Renderable();
    }
    
    // The VBO containing the 4 vertices of the particles.
    static const float data[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f
    };
    
    unsigned int indexes[] = {
        0, 2, 1, 1, 2, 3
    };
    
    if(renderable->vertexBuffer == 0) {
        VertexFormat* vertexFormat = renderSystem->CreateVertexFormat();
        vertexFormat->Initialize();
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 3, 0);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD0, 2, 3);
        
        renderable->vertexBuffer = renderSystem->CreateVertexBuffer();
        renderable->vertexBuffer->Create(vertexFormat, 4, (float*)data, false);
    }
    
    if(renderable->indexBuffer == 0) {
        renderable->indexBuffer = renderSystem->CreateIndexBuffer();
        renderable->indexBuffer->Create(6, indexes);
    }
}

void BillboardComponent::PreRender(Scene* scene) {
    CameraComponent* camera = scene->camera;
    matrix4 viewMatrix = camera->GetViewMatrix();
    
    // The VBO containing the 4 vertices of the particles.
    float data[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5, 0.0f, 1.0f, 0.0f
    };
    
    vector3 right = vector3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    vector3 up = vector3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
    
    for(int i = 0; i < 4; i++) {
        int offset = i * 5;
        vector3 df = vector3(data[offset + 0], data[offset + 1], data[offset + 2]);
        vector3 v = GetTransform()->GetWorldPosition() + right * df.x * m_size + up * df.y * m_size;
        
        data[offset + 0] = v.x;
        data[offset + 1] = v.y;
        data[offset + 2] = v.z;
    }
    
    renderable->vertexBuffer->SetData(20, 0, data);
    
    renderable->diffuseTexture = m_texture;
}

void BillboardComponent::Serialize(DataRecord* record) {
    TransformableComponent::Serialize(record);
    
    record->Set("Billboard.size", new Variant(m_size));
    if(m_texture) {
        record->Set("Billboard.texture", new Variant(m_texture->GetResource()->filename));
    }
    else {
        record->Set("Billboard.texture", new Variant(std::string()));
    }
}

void BillboardComponent::Deserialize(DataRecord* record) {
    TransformableComponent::Deserialize(record);
    
    std::string texture = record->Get("Billboard.texture")->AsString();
    m_size = record->Get("Billboard.size")->AsFloat();
    
    if(texture.length()) {
        ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
        m_texture = (Texture2D*)resourceSystem->LoadResource(texture, "Texture2D");
        this->Create(m_texture, m_size);
    }
}
