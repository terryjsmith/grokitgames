#include "rotationgizmocomponent.h"

#include <IO/ResourceSystem.h>
#include <Core/Application.h>

RotationGizmoComponent::RotationGizmoComponent()
{
    this->applyLighting = false;

    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    Mesh* mesh = (Mesh*)resourceSystem->LoadResource("GizmoRotation.fbx", "Mesh");

    this->Initialize(mesh);
}
