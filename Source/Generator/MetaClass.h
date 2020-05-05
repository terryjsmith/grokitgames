
#ifndef metaclass_h
#define metaclass_h

#include <Core/GigaObject.h>

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
    };
    
    // Parameter types and type names
    std::vector<Parameter*> params;
    
    // Return type
    int32_t returnType;
    
    // Static / constructor
    bool isStatic;
    bool isConstructor;
    
    // Actual callable function
    CallableFunction func;
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
    std::vector<MetaFunction*> GetFunctions() { return m_functions; }
    
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
    
protected:
    // Create a new component from a class type
    template<typename T> static GigaObject* InternalCreateObject() { return new T; }
    
protected:
    // List of callable function names
    std::vector<MetaFunction*> m_functions;
    
    // Constructor for all meta GigaObject types
    typedef GigaObject* (*MetaConstructor)(void);
    
    // Constructor to create new instances
    MetaConstructor m_ctor;
};

#endif
