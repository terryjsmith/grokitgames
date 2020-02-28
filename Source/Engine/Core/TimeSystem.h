
#ifndef timesystem_h
#define timesystem_h

#include <Core/System.h>

#define GIGA_TICKS_PER_SECOND   20

GIGA_CLASS(Singleton)
class GIGA_API TimeSystem : public System {
public:
    TimeSystem();
    ~TimeSystem() = default;
    
    GIGA_CLASS_NAME("TimeSystem");
    
    /**
     * Get current timestamp (UTC)
     */
    GIGA_FUNCTION() static void GetTimestamp(struct timespec *ts);
    
    /**
     * Get difference between two timespecs
     */
    GIGA_FUNCTION() static timespec Diff(timespec* start, timespec* end);
    
    /**
     * Sleep
     */
    GIGA_FUNCTION() static void Sleep(int milliseconds);
    
    /**
     * Get current tick
     */
    GIGA_FUNCTION() int GetCurrentTick();
    
    /**
     * Get current time
     */
    void GetTime(tm* timestamp);
    
    /**
     * Get current offset time since start
     */
    float GetCurrentTickTime();
    
    /**
     * Set a manual "tick offset" for replaying
     */
    void SetTick(int tick) { m_overrideTick = tick; }
    
    /**
     * Get/set system startup time
     */
    timespec GetStartupTime() { return m_startupTime; }
    void SetStartupTime(timespec t) { m_startupTime = t; }
    
    /**
     * Set client time offset (for sync to server)
     */
    void SetClientOffset(double offset) { m_clientOffset = offset; }
    
protected:
    // Startup time of the system
    timespec m_startupTime;
    
    // Manual tick offset for replaying
    int m_overrideTick;
    
    // Client offset
    double m_clientOffset;
};

#endif
