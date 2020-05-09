
#include <Audio/Sound.h>
#include <Audio/AudioSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>

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
    
    FMOD::Sound* m_sound;
    FMOD_RESULT result = system->createSound((const char*)data, FMOD_OPENMEMORY, &info, &m_sound);
    if (result != FMOD_OK) {
        errorSystem->HandleError(new Error(Error::ERROR_WARN, "Error creating sound.", this->m_resource->filename));
        GIGA_ASSERT(false, "Error creating sound.");
    }
}
