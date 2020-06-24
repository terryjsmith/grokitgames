
#ifndef directory_h
#define directory_h

#include <Core/GigaObject.h>

GIGA_CLASS()
class GIGA_API Directory : public GigaObject {
public:
    Directory() = default;
    ~Directory();
    
    GIGA_CLASS_NAME("Directory");
    
    GIGA_FUNCTION() void Open(std::string path);
    
    /**
     * Get current working directory of application
     */
    GIGA_FUNCTION() static std::string GetCurrent();
    
    /**
     * Switch directories
     */
    GIGA_FUNCTION() static void SetCurrent(std::string path);
    
public:
    std::string path;
    Array<std::string> files;
    Array<Directory*> subdirectories;
};

#endif
