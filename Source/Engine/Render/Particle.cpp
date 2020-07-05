
#include <Render/Particle.h>

Particle::Particle() {
    transform = new Transform();
    state = ParticleState::PARTICLESTATE_DEAD;
    delta = 0.0f;
    frame = 0;
    velocity = vector3(0);
}

Particle::~Particle() {
    if(transform) {
        delete transform;
        transform = 0;
    }
}
