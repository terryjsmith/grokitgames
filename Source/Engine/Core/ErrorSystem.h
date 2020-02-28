
#ifndef errorsystem_h
#define errorsystem_h

#include <Core/System.h>
#include <Core/Error.h>
#include <IO/Resource.h>

typedef void(*ErrorHandlingFunction)(Error* err);

GIGA_CLASS(Singleton)
class GIGA_API ErrorSystem : public System {
public:
    ErrorSystem() = default;
    ~ErrorSystem() = default;
    
    /**
     * Initialize system
     */
    void Initialize();
    
    /**
     * Handle error
     */
    void HandleError(Error* err);
    
    /**
     * Register error handler
     */
    void RegisterErrorHandler(int level, ErrorHandlingFunction func);
    
    class ErrorHandler {
    public:
        int level;
        ErrorHandlingFunction func;
    };
    
protected:
    // Error handlers
    std::vector<ErrorHandler*> m_handlers;
};

#endif
