
#include <Scripting/ScriptComponent.h>
#include <Core/DataRecord.h>

void ScriptComponent::Serialize(DataRecord* record) {
    record->Set("className", new Variant(this->className));
}

void ScriptComponent::Deserialize(DataRecord* record) {
    this->className = record->Get("className")->AsString();
}
