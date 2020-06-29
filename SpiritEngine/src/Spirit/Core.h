#pragma once

#ifdef SPIRIT_PLATFORM_WINDOWS
    #ifdef SPIRIT_BUILD_DLL
        #define SPIRIT_API __declspec(dllexport)
    #else
        #define SPIRIT_API __declspec(dllimport)
    #endif
#else
    #error Spirit Engine only supports Windows!
#endif
