
#ifndef audiosystem_h
#define audiosystem_h

#include <Core/System.h>
#include <Audio/Sound.h>
#include <Core/MathTypes.h>
#include <fmod.hpp>

GIGA_CLASS(Singleton)
class GIGA_API AudioSystem : public System {
public:
    AudioSystem() = default;
    ~AudioSystem() = default;
    
    GIGA_CLASS_NAME("AudioSystem");
    
    void Initialize();
    void Update(float delta);
    
    FMOD::System* GetSubSystem() { return m_system; }
    
    /**
     * Play a sound (2D)
     */
    GIGA_FUNCTION() void PlaySound(Sound* sound);
    
    /**
     * Set 3D position
     */
    void SetWorldPosition(vector3 position);
    
protected:
    FMOD::System* m_system;
};

#endif
