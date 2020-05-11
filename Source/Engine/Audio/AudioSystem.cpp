
#include <Audio/AudioSystem.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>
#include <Core/World.h>
#include <Audio/AudioComponent.h>

void AudioSystem::Initialize() {
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    FMOD_RESULT result;

    result = FMOD::System_Create(&m_system);
    if (result != FMOD_OK) {
        errorSystem->HandleError(new Error(Error::ERROR_WARN, "FMOD initialization failure", FMOD_ErrorString(result)));
        GIGA_ASSERT(false, "FMOD error");
    }

    result = m_system->init(512, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        errorSystem->HandleError(new Error(Error::ERROR_WARN, "FMOD initialization failure", FMOD_ErrorString(result)));
        GIGA_ASSERT(false, "FMOD error");
    }
}

void AudioSystem::PlaySound(Sound* sound) {
    FMOD::Channel* channel;
    sound->sound->setMode(FMOD_OPENMEMORY | FMOD_2D | FMOD_LOOP_OFF);
    m_system->playSound(sound->sound, 0, false, &channel);
}

void AudioSystem::Update(float delta) {
    // Add to any existing script components
    World* world = World::GetInstance();
    std::vector<AudioComponent*> components = world->FindComponents<AudioComponent>();
    std::vector<AudioComponent*>::iterator i = components.begin();
    for (; i != components.end(); i++) {
        
    }
}
