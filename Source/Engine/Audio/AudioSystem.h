
#ifndef audiosystem_h
#define audiosystem_h

#include <Core/System.h>
#include <fmod.hpp>

GIGA_CLASS(Singleton)
class GIGA_API AudioSystem : public System {
public:
    AudioSystem() = default;
    ~AudioSystem() = default;
    
    GIGA_CLASS_NAME("AudioSystem");
    
    void Initialize();
    void Update(float delta) { }
    
    FMOD::System* GetSubSystem() { return m_system; }
    
protected:
    FMOD::System* m_system;
};

#endif
