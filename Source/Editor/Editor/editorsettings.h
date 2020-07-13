#ifndef EDITORSETTINGS_H
#define EDITORSETTINGS_H

#include <Core/Array.h>
#include <Scripting/MonoObjects.h>

class EditorSettings {
public:
    EditorSettings();

    static EditorSettings* GetInstance();

    struct CustomField {
        std::string className;
        std::string field;
        std::string type;
        std::string action;
        std::string defaultValue;
    };

    // Get custom fields
    CustomField* GetCustomField(std::string className, std::string field);

    // Mono callbacks
    static void SetFieldType(MonoString* className, MonoString* field, MonoString* type);
    static void SetFieldAction(MonoString* className, MonoString* field, MonoString* action);
    static void SetFieldDefault(MonoString* className, MonoString* field, MonoString* value);

protected:
    // Set up callbacks for Mono integration
    void Initialize();

protected:
    // Singleton
    static EditorSettings* m_instance;

    // Custom fields
    Array<CustomField*> m_customFields;
};

#endif // EDITORSETTINGS_H
