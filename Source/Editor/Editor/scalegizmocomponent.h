#ifndef SCALEGIZMOCOMPONENT_H
#define SCALEGIZMOCOMPONENT_H

#include "gizmocomponent.h"

class ScaleGizmoComponent : public GizmoComponent
{
public:
    ScaleGizmoComponent();

    // Don't show anything here
    virtual void Serialize(DataRecord* record) { }
    virtual void Deserialize(DataRecord* record) { }
};

#endif // SCALEGIZMOCOMPONENT_H
