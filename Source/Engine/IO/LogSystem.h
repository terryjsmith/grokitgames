
#ifndef logsystem_h
#define logsystem_h

#include <Core/System.h>
#include <Core/Error.h>
#include <IO/Resource.h>

GIGA_CLASS(Singleton)
class GIGA_API LogSystem : public System {
public:
    LogSystem();
    ~LogSystem() = default;
    
    GIGA_CLASS_NAME("LogSystem");
    
    /**
     * Initialize
     */
    void Initialize();
    
    /**
     * Open error log
     */
    void Open(std::string filename);
    
    /**
     * Close log file
     */
    void Close();
    
    /**
     * Log something to output file
     */
    GIGA_FUNCTION() void Log(int type, std::string message, std::string details = "");
    
    /**
     * Default logging error hander
     */
    static void LogError(Error* error);
    
    /**
     * Set minimum debug level (should probably be MSG_WARN for maybe ERROR_INFO for production)
     */
    GIGA_FUNCTION() void SetLogLevel(int level) { m_logLevel = level; }
    
protected:
    // Current logging level
    uint32_t m_logLevel;
    
    // Error log
    Resource* m_errorLog;
};

#endif
