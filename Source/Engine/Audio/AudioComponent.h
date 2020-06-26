
#ifndef audiocomponent_h
#define audiocomponent_h

#include <Core/TransformableComponent.h>
#include <Audio/Sound.h>

GIGA_CLASS()
class GIGA_API AudioComponent : public TransformableComponent {
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

public:
    // Channel
    FMOD::Channel* channel;
    
protected:
    // Audio file
    Sound* m_sound;
};

#endif 
