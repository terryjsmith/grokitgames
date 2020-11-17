
#ifndef particlesystem_h
#define particlesystem_h

#include <Render/ParticleEmitterComponent.h>
#include <Core/System.h>

class GIGA_API ParticleSystem : public System {
public:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    GIGA_CLASS_NAME("ParticleSystem");
    
    virtual void Update(float delta);
};

#endif 
