
#include <Gui/GuiDropdown.h>

GuiDropdown::GuiDropdown() {
    m_currentIndex = -1;
}

void GuiDropdown::Add(std::string key, Variant* value) {
    m_values[key] = value;
}

void GuiDropdown::Remove(std::string key) {
    auto it = m_values.find(key);
    if(it != m_values.end()) {
        m_values.erase(it);
    }
}

int GuiDropdown::Size() {
    return((int)m_values.size());
}

std::string GuiDropdown::GetKey(int index) {
    GIGA_ASSERT(index < (int)m_values.size(), "Invalid index.");
    auto it = std::next(m_values.begin(), index);
    return(it->first);
}

Variant* GuiDropdown::GetValue(int index) {
    GIGA_ASSERT(index < (int)m_values.size(), "Invalid index.");
    auto it = std::next(m_values.begin(), index);
    return(it->second);
}

int GuiDropdown::GetSelectedItem() {
    return(m_currentIndex);
}

void GuiDropdown::SetSelectedItem(int index) {
    GIGA_ASSERT(index < m_values.size(), "Invalid offset.");
    GIGA_ASSERT(index >= 0, "Invalid offset.");
    
    m_currentIndex = index;
}

void GuiDropdown::SetSelectedItem(std::string key) {
    auto it = m_values.find(key);
    GIGA_ASSERT(it != m_values.end(), "Unable to find key.");
    
    m_currentIndex = std::distance(m_values.begin(), it);
}
