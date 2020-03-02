
#ifndef meta_h
#define meta_h

#include <Core/GigaObject.h>

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
    
    // Constructor for all meta GigaObject types
    typedef GigaObject* (*MetaConstructor)(void);
    
    // Constructor to create new instances
    MetaConstructor m_ctor;
};

}

#endif

