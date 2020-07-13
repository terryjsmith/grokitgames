#ifndef ROTATIONGIZMOCOMPONENT_H
#define ROTATIONGIZMOCOMPONENT_H

#include <Render/MeshComponent.h>

class RotationGizmoComponent : public MeshComponent
{
public:
    RotationGizmoComponent();

    // Don't show anything here
    virtual void Serialize(DataRecord* record) { }
    virtual void Deserialize(DataRecord* record) { }
};

#endif // ROTATIONGIZMOCOMPONENT_H
