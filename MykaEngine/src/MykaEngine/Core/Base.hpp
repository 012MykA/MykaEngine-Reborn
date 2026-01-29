#pragma once

#include "MykaEngine/Core/PlatformDetection.hpp"

#include <memory>

// Debugbreak support
#if defined(MYKA_PLATFORM_WINDOWS)
    #define MYKA_DEBUGBREAK() __debugbreak()
#elif defined(MYKA_PLATFORM_LINUX)
    #include <signal.h>
    #define MYKA_DEBUGBREAK() raise(SIGTRAP)
#else
    #error "Platform doesn't support debugbreak yet!"
#endif // Debugbreak support

// Debug
#ifdef MYKA_DEBUG
	#define MYKA_ENABLE_ASSERTS
#endif // End Debug

// Dist
#ifndef MYKA_DIST
	#define MYKA_ENABLE_VERIFY
#endif // End Dist

#define BIT(x) (1 << x)

#define MYKA_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

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

#include "MykaEngine/Core/Log.hpp"
#include "MykaEngine/Core/Assert.hpp"
