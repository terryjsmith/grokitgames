
#ifndef particle_h
#define particle_h

#include <Render/RenderComponent.h>

GIGA_CLASS()
class GIGA_API Particle : public GigaObject {
public:
    Particle();
    ~Particle();
    
    GIGA_CLASS_NAME("Particle");
    
    enum ParticleState {
        PARTICLESTATE_NEW = 1,
        PARTICLESTATE_ALIVE,
        PARTICLESTATE_DEAD
    };
    
public:
    // Transform for the particle
    GIGA_VARIABLE(Get) Transform* transform;
    
    // Current particle state
    GIGA_VARIABLE(Get, Set) int state;
    
    // Current timestamp / delta since emitted
    GIGA_VARIABLE(Get) float delta;
    
    // Current particle velocity
    GIGA_VARIABLE(Get, Set) vector3 velocity;
    
    // Current frame (animations)
    GIGA_VARIABLE(Get, Set) int frame;
};

#endif
