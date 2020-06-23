
#ifndef guidropdown_h
#define guidropdown_h

#include <Core/Giga.h>
#include <Core/Variant.h>
#include <Gui/GuiWidget.h>

GIGA_CLASS()
class GIGA_API GuiDropdown : public GuiWidget {
public:
    GuiDropdown();
    ~GuiDropdown() = default;
    
    GIGA_CLASS_NAME("GuiDropdown");
    
    /**
     * Initialize
     */
    GIGA_FUNCTION() virtual void Initialize() { }
    
    /**
     * Add an item to the dropdown
     */
    GIGA_FUNCTION() void Add(std::string key, Variant* value);
    
    /**
     * Remove an item from the dropdown
     */
    GIGA_FUNCTION() void Remove(std::string key);
    
    /**
     * Get the size of the dropdown
     */
    GIGA_FUNCTION() int Size();
    
    /**
     * Get key/value at the index
     */
    std::string GetKey(int index);
    Variant* GetValue(int index);
    
    /**
     * Get/set selected item
     */
    GIGA_FUNCTION() int GetSelectedItem();
    GIGA_FUNCTION() void SetSelectedItem(int index);
    GIGA_FUNCTION() void SetSelectedItem(std::string key);
    
    
protected:
    // List of key/value pairs
    std::map<std::string, Variant*> m_values;
    
    // Current index
    int m_currentIndex;
};

#endif
