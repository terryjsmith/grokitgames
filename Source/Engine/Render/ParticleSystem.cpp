
#include <Render/ParticleSystem.h>
#include <Core/World.h>

void ParticleSystem::Update(float delta) {
    World* world = World::GetInstance();
    Array<ParticleEmitterComponent*> emitters = world->FindComponents<ParticleEmitterComponent>();
    auto it = emitters.begin();
    for(; it != emitters.end(); it++) {
        // Check for emission
        float lastEmission = (*it)->GetLastEmission(delta);
        float rate = (*it)->GetEmissionRate();
        float emissionsPerSecond = 1.0f / rate;
        float lifespan = (*it)->GetLifespan();
        
        // Get particles
        Array<Particle*> particles = (*it)->GetParticles(Particle::PARTICLESTATE_ALIVE);
        auto pi = particles.begin();
        for(; pi != particles.end(); pi++) {
            (*pi)->delta += delta;
            (*pi)->transform->Move((*pi)->velocity * delta);
            
            // "Kill" old particles
            if((*pi)->delta >= lifespan) {
                (*pi)->state = Particle::PARTICLESTATE_DEAD;
            }
        }
        
        // Move particles from new to alive
        particles = (*it)->GetParticles(Particle::PARTICLESTATE_NEW);
        pi = particles.begin();
        for(; pi != particles.end(); pi++) {
            (*pi)->state = Particle::PARTICLESTATE_ALIVE;
        }
        
        // Emit new particles
        if(lastEmission > emissionsPerSecond) {
            (*it)->Emit();
        }
    }
}
