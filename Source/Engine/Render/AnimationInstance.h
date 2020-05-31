
#ifndef animationinstance_h
#define animationinstance_h

#include <Render/Animation.h>

/**
 * An instance of a running animation
 */
class GIGA_API AnimationInstance {
public:
    AnimationInstance() = default;
    ~AnimationInstance() = default;
    
public:
    // The animation
    Animation* animation;
    
    // Current time
    float currentTime;
    
    // Loop
    bool looping;
    
    // Bones
    std::map<std::string, matrix4> bones;
};

#endif 
