#pragma once

#include <memory>

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

// DLL support
#ifdef MYKA_DYNAMIC_LINK
    #error "Dynamic linking is not supported! Use static link instead."
#endif // End DLL support

// Debug
#ifdef MYKA_DEBUG
    #define MYKA_ENABLE_ASSERTS
#endif // End Debug

// Asserts
#ifdef MYKA_ENABLE_ASSERTS
    #define MYKA_ASSERT(x, ...) { if(!(x)) { MYKA_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define MYKA_CORE_ASSERT(x, ...) { if(!(x)) { MYKA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define MYKA_ASSERT(x, ...)
    #define MYKA_CORE_ASSERT(x, ...)
#endif // End Asserts


#define BIT(x) (1 << x)

#define MYKA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


namespace Myka
{
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
} // namespace Myka