
#ifndef particleemittercomponent_h
#define particleemittercomponent_h

#include <Render/MeshComponent.h>
#include <Render/Particle.h>
#include <Render/CameraComponent.h>

GIGA_CLASS()
class GIGA_API ParticleEmitterComponent : public MeshComponent {
public:
    ParticleEmitterComponent();
    ~ParticleEmitterComponent() = default;
    
    GIGA_CLASS_NAME("ParticleEmitterComponent");
    
    /**
     * Initialize emitter
     */
    void Initialize(Texture2D* texture, float size, int maxCount);
    
    /**
     * Emit a new particle (manually)
     */
    GIGA_FUNCTION() Particle* Emit();
    
    /**
     * Get active particles
     */
    GIGA_FUNCTION() Array<Particle*> GetParticles(int state);
    
    /**
     * Sort particles from farthest to closest from the camera
     */
    Array<Particle*> SortParticles(Array<Particle*>& particles, CameraComponent* camera);
    
    /**
     * Any pre-render work that needs to be done can be done here
     */
    virtual void PreRender(Scene* scene);
    
    /**
     * Control emission rate
     */
    float GetEmissionRate() { return m_rate; }
    float GetLastEmission(float delta);
    
    /**
     * Get life span
     */
    float GetLifespan() { return m_lifespan; }
    
protected:
    // Particles
    Array<Particle*> m_particles;
    
    // Size of billboard object
    float m_size;
    
    // Emission rate
    float m_rate;
    float m_last;
    
    // Total lifespan
    float m_lifespan;
    
    // Emission velocity
    vector3 m_velocity;
};

#endif
