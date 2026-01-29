// Platform detection
#ifdef _WIN32

    #ifdef _WIN64
        #define MYKA_PLATFORM_WINDOWS
    #else
        #error "Windows x86 is not supported!"
    #endif

#elif defined(__linux__)
    #define MYKA_PLATFORM_LINUX
    #error "Linux platform is not supported!"

#elif defined(__APPLE__) || defined(__MACH__)
    #define MYKA_PLATFORM_APPLE
    #error "Apple platform is not supported!"

#elif defined(__ANDROID__)
    #define MYKA_PLATFORM_ANDROID
    #error "Android platform is not supported!"

#else
    #error "Unknown platform!"

#endif // End Platform detection
