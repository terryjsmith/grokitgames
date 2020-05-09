
#ifndef sound_h
#define sound_h

#include <IO/ResourceObject.h>
#include <fmod.hpp>

GIGA_CLASS()
class GIGA_API Sound : public ResourceObject {
public:
    Sound() = default;
    ~Sound() = default;
    
    GIGA_CLASS_NAME("Sound");
    
    void ProcessData();
    
protected:
    FMOD::Sound* m_sound;
};

#endif
