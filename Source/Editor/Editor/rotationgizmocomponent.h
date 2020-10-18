#ifndef ROTATIONGIZMOCOMPONENT_H
#define ROTATIONGIZMOCOMPONENT_H

#include "gizmocomponent.h"

class RotationGizmoComponent : public GizmoComponent
{
public:
    RotationGizmoComponent();

    // Don't show anything here
    virtual void Serialize(DataRecord* record) { }
    virtual void Deserialize(DataRecord* record) { }
};

#endif // ROTATIONGIZMOCOMPONENT_H
