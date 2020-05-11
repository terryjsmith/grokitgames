
#ifndef sound_h
#define sound_h

#include <IO/ResourceObject.h>
#include <fmod.hpp>

GIGA_CLASS()
class GIGA_API Sound : public ResourceObject {
public:
    Sound();
    ~Sound();
    
    GIGA_CLASS_NAME("Sound");
    
    void ProcessData();
    
    /**
     * Set loop status
     */
    void SetLooping(bool looping);
    
    /**
     * Set 2D or 2D
     */
    void Set3D(bool p3d);
    
protected:
    /**
     * Set flags (after flag changes)
     */
    void SetFlags();
    
public:
    // Internal sound
    FMOD::Sound* sound;
    
protected:
    // Looping
    bool m_looping;
    
    // 2D or 3D
    bool m_3d;
};

#endif
