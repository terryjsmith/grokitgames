
#include <Audio/Sound.h>
#include <Audio/AudioSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>

Sound::Sound() {
    m_looping = false;
    m_3d = false;
}

Sound::~Sound() {
    if(sound) {
        sound->release();
        sound = 0;
    }
}

void Sound::ProcessData() {
    AudioSystem* audioSystem = GetSystem<AudioSystem>();
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    
    FMOD::System* system = audioSystem->GetSubSystem();
    
    // Get data
    unsigned char* data = this->m_resource->GetData();
    
    // Set info
    FMOD_CREATESOUNDEXINFO info;
    memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    
    info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    info.length = this->m_resource->filesize;
    
    FMOD_RESULT result = system->createSound((const char*)data, FMOD_OPENMEMORY, &info, &sound);
    if (result != FMOD_OK) {
        errorSystem->HandleError(new Error(Error::ERROR_WARN, "Error creating sound.", this->m_resource->filename));
        GIGA_ASSERT(false, "Error creating sound.");
    }
}

void Sound::SetFlags() {
    int flags = FMOD_OPENMEMORY;
    flags |= (m_3d) ? FMOD_3D : FMOD_2D;
    flags |= (m_looping) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    
    sound->setMode(flags);
}

void Sound::SetLooping(bool looping) {
    m_looping = looping;
    SetFlags();
}

void Sound::Set3D(bool p3d) {
    m_3d = p3d;
    SetFlags();
}
