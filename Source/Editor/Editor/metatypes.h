#ifndef METATYPES_H
#define METATYPES_H

#include <QObject>

#include <Core/DataRecord.h>
#include <Core/GigaObject.h>
#include <Core/Entity.h>
#include "editormode.h"

Q_DECLARE_METATYPE(GigaObject*);
Q_DECLARE_METATYPE(DataRecord*);
Q_DECLARE_METATYPE(Entity*);
Q_DECLARE_METATYPE(EditorMode*);

#endif // METATYPES_H
