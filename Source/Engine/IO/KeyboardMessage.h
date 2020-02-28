
#ifndef keyboardmessage_h
#define keyboardmessage_h

/**
 * Keyboard event data
 */
GIGA_CLASS()
class GIGA_API KeyboardMessage : public GigaObject {
public:
    GIGA_CLASS_NAME("KeyboardMessage");
    
    KeyboardMessage() = default;
    KeyboardMessage(int key, int action) : m_key(key), m_action(action) { }
    
    GIGA_FUNCTION() int GetKey() { return m_key; }
    GIGA_FUNCTION() int GetAction() { return m_action; }
    
protected:
    // Key that was pressed or released
    int m_key;
    int m_action;
};

#endif
