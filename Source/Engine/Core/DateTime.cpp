
#include <Core/DateTime.h>

std::string DateTime::ToString() {
    // Prepare output string
    std::string output;
    output.resize(100);
    
    // Convert timestamp to local time
    tm* local = localtime(&m_time);
    int written = sprintf((char*)output.data(), "%d-%02d-%02d %02d:%02d:%02d",
            local->tm_year + 1900,
            local->tm_mon,
            local->tm_mday,
            local->tm_hour,
            local->tm_min,
            local->tm_sec);
    output.resize(written);

    return(output);
}

DateTime* DateTime::GetCurrent() {
    DateTime* dt = new DateTime();
    time(&dt->m_time);
    return(dt);
}

uint64_t DateTime::GetTimestamp() {
    return(m_time);
}

int DateTime::GetYear() {
    tm* local = localtime(&m_time);
    return(local->tm_year + 1900);
}

int DateTime::GetMonth() {
    tm* local = localtime(&m_time);
    return(local->tm_mon);
}

int DateTime::GetDay() {
    tm* local = localtime(&m_time);
    return(local->tm_mday);
}

int DateTime::GetHour() {
    tm* local = localtime(&m_time);
    return(local->tm_hour);
}

int DateTime::GetMinute() {
    tm* local = localtime(&m_time);
    return(local->tm_min);
}

int DateTime::GetSecond() {
    tm* local = localtime(&m_time);
    return(local->tm_sec);
}
