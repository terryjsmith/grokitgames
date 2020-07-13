#ifndef TRANSLATIONGIZMOCOMPONENT_H
#define TRANSLATIONGIZMOCOMPONENT_H

#include <Render/MeshComponent.h>

class TranslationGizmoComponent : public MeshComponent
{
public:
    TranslationGizmoComponent();

    // Don't show anything here
    virtual void Serialize(DataRecord* record) { }
    virtual void Deserialize(DataRecord* record) { }
};

#endif // TRANSLATIONGIZMOCOMPONENT_H
