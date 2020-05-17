
#ifndef audiocomponent_h
#define audiocomponent_h

#include <Core/Component.h>
#include <Core/Transform.h>
#include <Audio/Sound.h>

GIGA_CLASS()
class GIGA_API AudioComponent : public Component {
public:
    AudioComponent();
    ~AudioComponent();
    
    GIGA_CLASS_NAME("AudioComponent");
    
    /**
     * Initialize from audio clip
     */
    void Initialize(Sound* clip);
    
    /**
     * Play
     */
    GIGA_FUNCTION() void Play();
    
    /**
     * Get transform
     */
    GIGA_FUNCTION() Transform* GetTransform() { return m_transform; }
    
public:
    // Channel
    FMOD::Channel* channel;
    
protected:
    // Audio file
    Sound* m_sound;
    
    // Transform
    Transform* m_transform;
};

#endif 
