
#include <Render/Frustum.h>

int Frustum::Intersects(vector3& point) {
    for (int i = 0; i < 6; i++) {
        if (planes[i].Distance(point) > 0)
            return(0);
    }
    return(1);
}

bool Frustum::Intersects(Sphere* s) {
    for (int p = 0; p < 6; p++) {
        float distance = planes[p].Distance(s->center);
        if(s->Intersects(&planes[p]) == false && distance > 0)
            return(false);
    }
    return(1);
}
