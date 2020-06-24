
#ifndef metaclass_h
#define metaclass_h

#include <Core/GigaObject.h>
#include <Core/Meta.h>
#include <Core/Array.h>

/**
 * Definitions of variables and functions
 */
struct MetaFunction {
public:
    // Name of the function
    std::string name;
    
    struct Parameter {
        std::string name;
        uint32_t type;
        std::string typeStr;
        bool found;
    };
    
    // Parameter types and type names
    Array<Parameter*> params;
    
    // Return type
    int32_t returnType;
    std::string strReturnType;
    
    // Static / constructor
    bool isStatic;
    bool isConstructor;
    
    // Actual callable function
    CallableFunction func;
    
    bool found;
};

struct MetaVariable {
    // Variable / property name
    std::string name;
    
    // Type info
    uint32_t type;
    std::string typeStr;
    
    // Getter / setter functions
    GetterFunction getter;
    SetterFunction setter;
    
    bool found;
};

/**
 * Definition of an object
 */
class MetaClass {
public:
    MetaClass();
    ~MetaClass();
    
    /**
     * Add a function definition
     */
    bool AddFunction(MetaFunction* fn, bool checkSignature = true);
    
    /**
     * Find a function
     */
    MetaFunction* FindFunction(std::string name, int offset = 0);
    
    /**
     * Get all functions
     */
    Array<MetaFunction*> GetFunctions() { return m_functions; }
    
    /**
     * Add a property
     */
    bool AddVariable(MetaVariable* prop, bool checkSignature = true);
    
    /**
     * Find a property
     */
    MetaVariable* FindVariable(std::string name);
    
    /**
     * Get all variables
     */
    Array<MetaVariable*> GetVariables() { return m_variables; }
    
    /**
     * Create an object of this type
     */
    GigaObject* CreateObject();
    
    /**
     * Register a construction
     */
    template<class T>
    void RegisterConstructor() {
        m_ctor = InternalCreateObject<T>;
    }
    
    /**
     * Clear
     */
    void Clear();
    
public:
    // Numeric object type ID
    uint32_t typeID;
    
    // String type name
    std::string name;
    
    // Is singleton type
    bool singleton;
    
    // Is this a templated class
    bool templated;
    
    // Public constructor?
    bool hasPublicConstructor;
    
    bool found;
    
    Array<std::string> inheritsFrom;
    
protected:
    // Create a new component from a class type
    template<typename T> static GigaObject* InternalCreateObject() { return new T; }
    
protected:
    // List of callable function names
    Array<MetaFunction*> m_functions;
    
    // List of variables
    Array<MetaVariable*> m_variables;
    
    // Constructor for all meta GigaObject types
    typedef GigaObject* (*MetaConstructor)(void);
    
    // Constructor to create new instances
    MetaConstructor m_ctor;
};

#endif
