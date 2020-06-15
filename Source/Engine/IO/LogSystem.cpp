
#include <IO/LogSystem.h>
#include <Core/ErrorSystem.h>
#include <Core/Application.h>
#include <Core/DateTime.h>

LogSystem::LogSystem() {
    m_logLevel = Error::MSG_INFO;
    m_errorLog = 0;
}

void LogSystem::Initialize() {
    // Register to receive errors
    ErrorSystem* errorSystem = GetSystem<ErrorSystem>();
    errorSystem->RegisterErrorHandler(Error::MSG_INFO, LogError);
    errorSystem->RegisterErrorHandler(Error::MSG_WARN, LogError);
    errorSystem->RegisterErrorHandler(Error::MSG_DEBUG, LogError);
    errorSystem->RegisterErrorHandler(Error::MSG_FATAL, LogError);
}

void LogSystem::Open(std::string filename) {
    if(m_errorLog) {
        delete m_errorLog;
    }
    
    m_errorLog = new Resource();
    m_errorLog->Initialize(filename, Resource::FILEMODE_APPEND);
}

void LogSystem::Close() {
    if(m_errorLog) {
        m_errorLog->Close();
        delete m_errorLog;
        m_errorLog = 0;
    }
}

void LogSystem::Log(int type, std::string message, std::string details) {
    std::string output;

    if(type < m_logLevel) {
        return;
    }
        
    // Prepend date/time
    DateTime* dt = DateTime::GetCurrent();
    output += "[" + dt->ToStr() + "] ";
        
    // Prepend some info
    switch(type) {
        case Error::MSG_INFO:
            output += "[INFO ]: ";
            break;
        case Error::MSG_DEBUG:
            output += "[DEBUG]: ";
            break;
        case Error::MSG_WARN:
            output += "[WARN ]: ";
            break;
        case Error::MSG_FATAL:
            output += "[FATAL]: ";
            break;
        default:
            output += "[INFO ]: ";
    }
        
    // Append the message
    output += message;
        
    // Then any details
    if(details.length()) {
        output += " (" + details + ")";
    }
        
    printf("%s\n", output.c_str());

#ifdef WIN32
    OutputDebugString(output.c_str());
#endif
        
    m_errorLog->WriteLine(output);
        
    delete dt;
}

void LogSystem::LogError(Error* error) {
    LogSystem* logSystem = GetSystem<LogSystem>();
    logSystem->Log(error->level, error->message, error->detail);
}
