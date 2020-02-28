
#ifndef giga_h
#define giga_h

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>

#ifndef GIGA_API
    #define GIGA_API
    #define GIGA_CLASS(...)
    #define GIGA_FUNCTION(...)
    #define GIGA_VARIABLE(...)
#endif

#define GIGA_ASSERT(test, message) { if((test) == false) printf(message); assert(test); }
#define GIGA_CLASS_NAME(str) virtual std::string GetGigaName() { return str; }

#endif
