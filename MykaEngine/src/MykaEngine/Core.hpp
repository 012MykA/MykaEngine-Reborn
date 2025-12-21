#pragma once

#ifdef MYKA_PLATFORM_WINDOWS

#ifdef  MYKA_BUILD_DLL
#define MYKA_API _declspec(dllexport)

#else
#define MYKA_API _declspec(dllimport)
#endif

#else
#error MykaEngine only supports Windows!
#endif

#ifdef MYKA_ENABLE_ASSERTS
#define MYKA_ASSERT(x, ...) { if(!(x)) { MYKA_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define MYKA_CORE_ASSERT(x, ...) { if(!(x)) { MYKA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define MYKA_ASSERT(x, ...)
#define MYKA_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define MYKA_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)