
#ifndef errorsystem_h
#define errorsystem_h

#include <Core/System.h>
#include <Core/Error.h>

typedef void(*ErrorHandlingFunction)(Error* err);

GIGA_CLASS(Singleton)
class GIGA_API ErrorSystem : public System {
public:
    ErrorSystem() = default;
    ~ErrorSystem() = default;
    
    GIGA_CLASS_NAME("ErrorSystem");
    
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
    Array<ErrorHandler*> m_handlers;
};

#endif
