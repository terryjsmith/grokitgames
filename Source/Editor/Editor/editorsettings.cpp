#include "editorsettings.h"

#include <mono/jit/jit.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Application.h>

EditorSettings* EditorSettings::m_instance = 0;

EditorSettings::EditorSettings() {

}

void EditorSettings::Initialize() {
    ScriptingSystem* scriptingSystem = GetSystem<ScriptingSystem>();
    scriptingSystem->AddInternalCall("GIGA.Editor::SetFieldType", (void*)&EditorSettings::SetFieldType);
    scriptingSystem->AddInternalCall("GIGA.Editor::SetFieldAction", (void*)&EditorSettings::SetFieldAction);
    scriptingSystem->AddInternalCall("GIGA.Editor::SetFieldDefault", (void*)&EditorSettings::SetFieldDefault);
}

EditorSettings::CustomField* EditorSettings::GetCustomField(std::string className, std::string field) {
    auto it = m_customFields.begin();
    for(; it != m_customFields.end(); it++) {
        if((*it)->className == className && (*it)->field == field) {
            return(*it);
        }
    }

    return(0);
}

EditorSettings* EditorSettings::GetInstance() {
    if(m_instance == 0) {
        m_instance = new EditorSettings();
        m_instance->Initialize();
    }

    return(m_instance);
}

void EditorSettings::SetFieldType(MonoString* className, MonoString* field, MonoString* type) {
    std::string strClassName = mono_string_to_utf8(className);
    std::string strFieldName = mono_string_to_utf8(field);

    EditorSettings* editor = EditorSettings::GetInstance();
    CustomField* cf = editor->GetCustomField(strClassName, strFieldName);
    if(cf == 0) {
        cf = new CustomField();
        cf->className = strClassName;
        cf->field = strFieldName;

        editor->m_customFields.push_back(cf);
    }

    cf->type = mono_string_to_utf8(type);
}

void EditorSettings::SetFieldAction(MonoString* className, MonoString* field, MonoString* action) {
    std::string strClassName = mono_string_to_utf8(className);
    std::string strFieldName = mono_string_to_utf8(field);

    EditorSettings* editor = EditorSettings::GetInstance();
    CustomField* cf = editor->GetCustomField(strClassName, strFieldName);
    if(cf == 0) {
        cf = new CustomField();
        cf->className = strClassName;
        cf->field = strFieldName;

        editor->m_customFields.push_back(cf);
    }

    cf->action = mono_string_to_utf8(action);
}

void EditorSettings::SetFieldDefault(MonoString* className, MonoString* field, MonoString* value) {
    std::string strClassName = mono_string_to_utf8(className);
    std::string strFieldName = mono_string_to_utf8(field);

    EditorSettings* editor = EditorSettings::GetInstance();
    CustomField* cf = editor->GetCustomField(strClassName, strFieldName);
    if(cf == 0) {
        cf = new CustomField();
        cf->className = strClassName;
        cf->field = strFieldName;

        editor->m_customFields.push_back(cf);
    }

    cf->defaultValue = mono_string_to_utf8(value);
}
