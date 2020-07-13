#include "translationgizmocomponent.h"
#include <IO/ResourceSystem.h>
#include <Core/Application.h>

TranslationGizmoComponent::TranslationGizmoComponent()
{
    this->applyLighting = false;

    ResourceSystem* resourceSystem = GetSystem<ResourceSystem>();
    Mesh* mesh = (Mesh*)resourceSystem->LoadResource("GizmoTranslation.fbx", "Mesh");

    this->Initialize(mesh);
}
