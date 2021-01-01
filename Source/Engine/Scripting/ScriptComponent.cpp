
#include <Scripting/ScriptComponent.h>
#include <Core/DataRecord.h>

void ScriptComponent::Serialize(DataRecord* record) {
    Component::Serialize(record);
    
    record->Set("className", new Variant(this->className));
}

void ScriptComponent::Deserialize(DataRecord* record) {
    Component::Deserialize(record);
    
    this->className = record->Get("className")->AsString();
}
