
#include <Render/AABB.h>

void AABB::Create(vector3 min, vector3 max) {
    this->min = min;
    this->max = max;

    points[POINT_NEARBOTTOMLEFT] = vector3(min.x, min.y, min.z);
    points[POINT_NEARBOTTOMRIGHT] = vector3(max.x, min.y, min.z);
    points[POINT_NEARTOPLEFT] = vector3(min.x, max.y, min.z);
    points[POINT_NEARTOPRIGHT] = vector3(max.x, max.y, min.z);
    points[POINT_FARBOTTOMLEFT] = vector3(min.x, min.y, max.z);
    points[POINT_FARBOTTOMRIGHT] = vector3(max.x, min.y, max.z);
    points[POINT_FARTOPLEFT] = vector3(min.x, max.y, max.z);
    points[POINT_FARTOPRIGHT] = vector3(max.x, max.y, max.z);
}
