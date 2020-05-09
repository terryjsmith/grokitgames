
#include <Audio/AudioSystem.h>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>

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
