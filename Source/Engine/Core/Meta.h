
#ifndef meta_h
#define meta_h

#include <Core/GigaObject.h>

typedef Variant*(*CallableFunction)(GigaObject* obj, int argc, Variant** argv);
typedef Variant*(*GetterFunction)(GigaObject* obj);
typedef void(*SetterFunction)(GigaObject* obj, Variant* value);

namespace Meta {

struct Function {
    // Name of the function
    std::string name;
    
    // Static / constructor
    bool isStatic;
    bool isConstructor;
    
    // Actual callable function
    CallableFunction func;
};

struct Variable {
    // Var name
    std::string name;
    
    // Getter / setter
    GetterFunction getter;
    SetterFunction setter;
};

class GIGA_API Class : public GigaObject {
public:
    Class();
    ~Class();
    
    GIGA_CLASS_NAME("Class");
    
    /**
     * Add a function definition
     */
    void AddFunction(Meta::Function* fn);
    
    /**
     * Find a function
     */
    Meta::Function* FindFunction(std::string name);
    
    /**
     * Get all functions
     */
    std::vector<Meta::Function*> GetFunctions() { return m_functions; }
    
    /**
     * Add a variable definition
     */
    void AddVariable(Meta::Variable* fn);
    
    /**
     * Find a variable
     */
    Meta::Variable* FindVariable(std::string name);
    
    /**
     * Get all variables
     */
    std::vector<Meta::Variable*> GetVariables() { return m_variables; }

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
    
public:
    // Numeric object type ID
    uint32_t typeID;
    
    // String type name
    std::string name;
    
    // Is singleton type
    bool singleton;

    // Public constructor?
    bool hasPublicConstructor;
    
protected:
    // Create a new component from a class type
    template<typename T> static GigaObject* InternalCreateObject() { return new T; }
    
protected:
    // List of callable function names
    std::vector<Meta::Function*> m_functions;
    
    // List of variables
    std::vector<Meta::Variable*> m_variables;
    
    // Constructor for all meta GigaObject types
    typedef GigaObject* (*MetaConstructor)(void);
    
    // Constructor to create new instances
    MetaConstructor m_ctor;
};

}

#endif

