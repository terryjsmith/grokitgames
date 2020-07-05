
#include <Render/ParticleEmitterComponent.h>
#include <Render/RenderSystem.h>
#include <Core/Application.h>

ParticleEmitterComponent::ParticleEmitterComponent() {
    instances = 0;
    instanced = true;
    m_size = 0;
    m_rate = 5.0f;
    m_velocity = vector3(0, 1, 0);
    m_lifespan = 3.0f;
}

void ParticleEmitterComponent::Initialize(Texture2D* texture, float size, int maxCount) {
    m_size = size;
    
    RenderSystem* renderSystem = GetSystem<RenderSystem>();
    if(renderable == 0) {
        renderable = new Renderable();
    }
    
    renderable->diffuseTexture = texture;
    
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
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_POSITION, 3, 0, 5);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_TEXCOORD0, 2, 3, 5);
        vertexFormat->AddVertexAttrib(VERTEXTYPE_ATTRIB_OFFSETS, 3, 20, 3);
        
        renderable->vertexBuffer = renderSystem->CreateVertexBuffer();
        renderable->vertexBuffer->Create(vertexFormat, 4, NULL, true, 20 /* 4 vertices * 5 elements */ + (3 * maxCount));
        renderable->vertexBuffer->SetData(20, 0, (float*)data);
    }
    
    if(renderable->indexBuffer == 0) {
        renderable->indexBuffer = renderSystem->CreateIndexBuffer();
        renderable->indexBuffer->Create(6, indexes);
    }
    
    // Initialize all particles to dead state
    m_particles.resize(maxCount);
    for(int i = 0; i < m_particles.size(); i++) {
        m_particles[i] = new Particle();
    }
}

Particle* ParticleEmitterComponent::Emit() {
    // Find an available particle slot
    auto it = m_particles.begin();
    for(; it != m_particles.end(); it++) {
        if((*it)->state == Particle::PARTICLESTATE_DEAD) {
            break;
        }
    }
    
    if(it == m_particles.end()) {
        GIGA_ASSERT(false, "No room for new particle.");
        return(0);
    }
    
    (*it)->delta = 0.0f;
    (*it)->frame = 0;
    (*it)->velocity = m_velocity;
    (*it)->state = Particle::PARTICLESTATE_NEW;
    (*it)->transform->SetWorldPosition(vector3(0));
    (*it)->transform->SetWorldRotation(quaternion(1, 0, 0, 0));
    (*it)->transform->SetWorldScaling(vector3(1));

    m_last = 0.0f;
    return(*it);
}

Array<Particle*> ParticleEmitterComponent::GetParticles(int state) {
    Array<Particle*> retval;
    auto it = m_particles.begin();
    for(; it != m_particles.end(); it++) {
        if((*it)->state == state) {
            retval.push_back(*it);
        }
    }
    
    return(retval);
}

void ParticleEmitterComponent::PreRender(Scene* scene) {
    // Transform the "base" vertex coordinates (-0.5 - 0.5) to face the camera
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
    
    // Set the centers of the particles in the vertex buffer
    Array<Particle*> particles = GetParticles(Particle::PARTICLESTATE_ALIVE);
    float* positions = new float[particles.size() * 3];
    for(int i = 0; i < particles.size(); i++) {
        int offset = i * 3;
        
        vector3 position = particles[i]->transform->GetWorldPosition();
        positions[offset + 0] = position.x;
        positions[offset + 1] = position.y;
        positions[offset + 2] = position.z;
    }
    
    renderable->vertexBuffer->SetData(particles.size() * 3, 20, positions);
    
    // Use the offsets for every 4th vertex (ie. every separate particle)
    VertexFormat* format = renderable->vertexBuffer->GetFormat();
    format->Bind();
    
    format->SetDivisor(0, 0); // Vertex position, use same vertex over and over
    format->SetDivisor(3, 0); // Same for texture coords
    format->SetDivisor(7, 1); // Advance once per vertex for offset data
    
    format->Unbind();
    
    // Save instance data
    this->instances = particles.size();
}

float ParticleEmitterComponent::GetLastEmission(float delta) {
    m_last += delta;
    return(m_last);
}
