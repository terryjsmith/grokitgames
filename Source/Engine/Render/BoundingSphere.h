
#ifndef boundingsphere_h
#define boundingsphere_h

#include <Render/Frustum.h>

class BoundingSphere {
public:
    BoundingSphere() = default;
    ~BoundingSphere() = default;
    
public:
    vector3 center;
    float radius;
};

#endif
