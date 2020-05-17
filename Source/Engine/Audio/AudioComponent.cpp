
#include <Audio/AudioComponent.h>
#include <Audio/AudioSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>
#include <Core/Entity.h>

AudioComponent::AudioComponent() {
    m_transform = new Transform();
    
    if(m_parent) {
        m_transform->SetParent(m_parent->GetTransform());
    }
}

AudioComponent::~AudioComponent() {
    if(m_transform) {
        delete m_transform;
        m_transform = 0;
    }
}

void AudioComponent::Initialize(Sound* clip) {
    m_sound = clip;
    m_sound->sound->setMode(FMOD_OPENMEMORY | FMOD_3D | FMOD_LOOP_OFF);
}

void AudioComponent::Play() {
    AudioSystem* audioSystem = GetSystem<AudioSystem>();
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    FMOD_RESULT result = audioSystem->GetSubSystem()->playSound(m_sound->sound, 0, true, &channel);
    if (result != FMOD_OK) {
        errorSystem->HandleError(new Error(Error::ERROR_WARN, "Error initializing sound.", m_sound->GetResource()->filename));
        GIGA_ASSERT(false, "Error initializing sound.");
    }
    
    FMOD_VECTOR vec;
    vec.x = 0;
    vec.y = 0;
    vec.z = 0;
    channel->set3DAttributes(&vec, NULL);
}
