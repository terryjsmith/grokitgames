
#ifndef plane_h
#define plane_h

#include <Core/Giga.h>
#include <Core/MathTypes.h>

/**
* Represent a plane in 3D space
*/
class GIGA_API Plane {
public:
    Plane() = default;
    ~Plane() = default;

public:
    /**
    * Create a plane from 3 points
    */
    void CreateFromPoints(vector3& p0, vector3& p1, vector3 &p2);

    /**
    * Get the distance to a point
    */
    float Distance(vector3& point);

public:
    vector3 abc;
    float d;
};

#endif
