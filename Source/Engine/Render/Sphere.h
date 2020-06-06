
#ifndef sphere_hpp
#define sphere_hpp

#include <Core/Giga.h>
#include <Core/MathTypes.h>
#include <Render/Plane.h>

class GIGA_API Sphere {
public:
    Sphere() = default;
    ~Sphere() = default;
    
    /**
     * Create a sphere
     */
    void Create(vector3 center, float radius);
    
    /**
     * Whether another sphere intersects this one
     */
    bool Intersects(Sphere* s);
    
    /**
     * Whether a point is inside this sphere
     */
    bool Inside(vector3 v);
    
    /**
     * Whether this sphere intersects with a plane
     */
    bool Intersects(Plane* p);
    
public:
    vector3 center;
    float radius;
};

#endif
