
#ifndef animation_h
#define animation_h

#include <Render/AnimationTransforms.h>

/**
 * A single animation
 */
class GIGA_API Animation {
public:
    Animation() = default;
    virtual ~Animation() = default;
    
    enum {
        ANIMATIONTYPE_SKELETAL = 1,
        ANIMATIONTYPE_MESH
    };
    
public:
    // Type of animation
    int type;
    
    // The name of the animation
    std::string name;
    
    // Duration of the animation in frames
    uint32_t duration;
    
    // The number of frames per second
    uint32_t framesPerSecond;
    
    // The transforms of this animation (indexed by node name)
    std::map<std::string, AnimationTransforms*> transforms;
    
    // Global inverse matrix
    matrix4 globalInverseMatrix;
};

#endif
