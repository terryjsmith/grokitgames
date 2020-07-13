#include "scalegizmocomponent.h"

#include <IO/ResourceSystem.h>
#include <Core/Application.h>

ScaleGizmoComponent::ScaleGizmoComponent()
{
    this->applyLighting = false;

    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    Mesh* mesh = (Mesh*)resourceSystem->LoadResource("GizmoScale.fbx", "Mesh");

    this->Initialize(mesh);
}
