
#ifndef animationtransforms_h
#define animationtransforms_h

#include <Core/Giga.h>
#include <Core/MathTypes.h>

class AnimationTransforms {
public:
    AnimationTransforms() = default;
    ~AnimationTransforms() = default;
    
public:
    struct Translation {
        int time;
        vector3 translation;
    };

    struct Scaling {
        int time;
        vector3 scaling;
    };

    struct Rotation {
        int time;
        quaternion rotation;
    };
    
public:
    // Which object this affects
    std::string nodeName;
    
    std::vector<Translation*> translations;
    std::vector<Scaling*> scaling;
    std::vector<Rotation*> rotations;
};

#endif
