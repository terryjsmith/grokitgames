
#ifndef animationframe_h
#define animationframe_h

#include <Core/Transform.h>

/**
 * A single object (bone/mesh) transform in the frame
 */
struct AnimationTransform {
    /**
     * The object (mesh or bone) index we are working on
     */
    uint32_t objectID;
    
    /**
     * Transforms for scaling, rotation and translation
     */
    vector3 scaling;
    vector3 translation;
    quaternion rotation;
};

/**
 * A frame of an animation
 */
class AnimationFrame {
public:
    AnimationFrame() = default;
    ~AnimationFrame() = default;
    
public:
    /**
     * The timestamp of this transformation (in ticks)
     */
    uint32_t time;
    
    /**
     * The objects affected by this and how
     */
    std::vector<AnimationTransform*> transforms;
};

#endif
