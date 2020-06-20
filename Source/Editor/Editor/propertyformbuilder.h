#ifndef PROPERTYFORMBUILDER_H
#define PROPERTYFORMBUILDER_H

#include <Core/DataRecord.h>
#include <QFormLayout>
#include <QObject>

Q_DECLARE_METATYPE(DataRecord*);

class PropertyFormBuilder : public QObject
{
public:
    PropertyFormBuilder() = default;

    static QFormLayout* GetFormLayout(DataRecord* record, QWidget* parent);

    static void clearLayout(QLayout *layout);
};

#endif // PROPERTYFORMBUILDER_H
