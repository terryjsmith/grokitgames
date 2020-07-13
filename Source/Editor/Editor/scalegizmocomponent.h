#ifndef SCALEGIZMOCOMPONENT_H
#define SCALEGIZMOCOMPONENT_H

#include <Render/MeshComponent.h>

class ScaleGizmoComponent : public MeshComponent
{
public:
    ScaleGizmoComponent();

    // Don't show anything here
    virtual void Serialize(DataRecord* record) { }
    virtual void Deserialize(DataRecord* record) { }
};

#endif // SCALEGIZMOCOMPONENT_H
