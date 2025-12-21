#pragma once

#ifdef MYKA_PLATFORM_WINDOWS

#if MYKA_DYNAMIC_LINK

#ifdef  MYKA_BUILD_DLL
#define MYKA_API __declspec(dllexport)
#else
#define MYKA_API __declspec(dllimport)
#endif // MYKA_BUILD_DLL

#else
#define MYKA_API
#endif // MYKA_DYNAMIC_LINK

#else
#error MykaEngine only supports Windows!
#endif // MYKA_PLATFORM_WINDOWS

#ifdef MYKA_ENABLE_ASSERTS
#define MYKA_ASSERT(x, ...) { if(!(x)) { MYKA_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define MYKA_CORE_ASSERT(x, ...) { if(!(x)) { MYKA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define MYKA_ASSERT(x, ...)
#define MYKA_CORE_ASSERT(x, ...)
#endif // MYKA_ENABLE_ASSERTS

#define BIT(x) (1 << x)

#define MYKA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)