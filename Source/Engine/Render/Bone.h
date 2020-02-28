
#ifndef bone_h
#define bone_h

#include <Core/Giga.h>

/**
 * Bones
 */
struct GIGA_API Bone {
    // The name of the bone in case we need it later
    std::string name;
    
    // Parent bone
    Bone* m_parent;
};

#endif
