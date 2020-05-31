
#ifndef bone_h
#define bone_h

#include <Core/Giga.h>
#include <Core/MathTypes.h>

/**
 * Bones
 */
struct GIGA_API Bone {
    // The name of the bone in case we need it later
    std::string name;
    
    // Bone offset matrix
    matrix4 offsetMatrix;
    
    // Final matrix
    matrix4 finalTransform;
};

#endif
