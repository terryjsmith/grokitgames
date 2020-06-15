
#ifndef datetime_h
#define datetime_h

#include <Core/GigaObject.h>

GIGA_CLASS()
class GIGA_API DateTime : public GigaObject {
public:
    DateTime() = default;
    ~DateTime() = default;
    
    GIGA_CLASS_NAME("DateTime");

    /**
     * Express time as string
     */
    GIGA_FUNCTION() std::string ToStr();
        
    /**
     * Get current datetime
     */
    GIGA_FUNCTION() static DateTime* GetCurrent();
        
    /**
     * Get timestamp
     */
    uint64_t GetTimestamp();
        
    /**
     * Get pieces
     */
    GIGA_FUNCTION() int GetYear();
    GIGA_FUNCTION() int GetMonth();
    GIGA_FUNCTION() int GetDay();
    GIGA_FUNCTION() int GetHour();
    GIGA_FUNCTION() int GetMinute();
    GIGA_FUNCTION() int GetSecond();
    
private:
    // Use time_t struct for more general date/time storage
    time_t m_time;
};

#endif
