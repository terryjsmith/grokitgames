#ifndef METATYPES_H
#define METATYPES_H

#include <QObject>

#include <Core/DataRecord.h>
#include <Core/GigaObject.h>
#include <Core/Entity.h>

Q_DECLARE_METATYPE(GigaObject*);
Q_DECLARE_METATYPE(DataRecord*);
Q_DECLARE_METATYPE(Entity*);

#endif // METATYPES_H
