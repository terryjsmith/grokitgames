
#ifndef scriptvariant_h
#define scriptvariant_h

#include <Core/Variant.h>

/**
* Extension of our base variant class to integrate with V8's types
*/
class GIGA_API ScriptVariant : public Variant {
public:
    // JS types
    enum JSType {
        VAR_FUNCTION = Variant::Type::VAR_LASTTYPE
    };

    /**
    * Checker
    */
    bool IsFunction() { return(m_type == VAR_FUNCTION); }

};

#endif
