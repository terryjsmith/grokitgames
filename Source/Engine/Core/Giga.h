
#ifndef giga_h
#define giga_h

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <time.h>

#ifdef GIGA_EXPORTS
    // We are building this library
    #define GIGA_API //__declspec(dllexport)
#else
    // We are using this library
    #define GIGA_API //__declspec(dllimport)
#endif

#endif

#ifndef GIGA_API
    #define GIGA_API
#endif

#ifndef GIGA_CLASS
    #define GIGA_CLASS(...)
    #define GIGA_FUNCTION(...)
    #define GIGA_VARIABLE(...)
#endif

#define GIGA_ASSERT(test, message) { if((test) == false) printf(message); assert(test); }
#define GIGA_CLASS_NAME(str) virtual std::string GetGigaName() { return str; }

#endif
