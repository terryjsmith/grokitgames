
#ifndef guiwidget_h
#define guiwidget_h

#include <Core/GigaObject.h>

/**
 * Base class for all other GUI objects in the engine
 */
GIGA_CLASS()
class GIGA_API GuiWidget : public GigaObject {
public:
    GuiWidget() = default;
    ~GuiWidget() = default;
    
    GIGA_CLASS_NAME("GuiWidget");
};

#endif
