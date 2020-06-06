
#include <Render/Sphere.h>

void Sphere::Create(vector3 center, float radius) {
    this->center = center;
    this->radius = radius;
}

bool Sphere::Intersects(Sphere* s) {
    float distance = glm::distance(this->center, s->center);
    if(distance < this->radius || distance < s->radius) {
        return(true);
    }
    
    return(false);
}

bool Sphere::Inside(vector3 v) {
    float distance = glm::distance(this->center, v);
    if(distance < this->radius) {
        return(true);
    }
    
    return(false);
}

bool Sphere::Intersects(Plane* p) {
    float distance = p->Distance(center);
    if(distance < this->radius) {
        return(true);
    }
    
    return(false);
}
