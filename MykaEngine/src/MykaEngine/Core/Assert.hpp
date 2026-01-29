#pragma once

#include "Base.hpp"
#include "Log.hpp"

// Asserts
#ifdef MYKA_ENABLE_ASSERTS
    #define MYKA_EXPAND_MACRO(x) x
    #define MYKA_GET_ASSERT_MACRO(_1, _2, NAME, ...) NAME

    #define MYKA_ASSERT_WITH_MSG(x, ...) { if(!(x)) { MYKA_ERROR("Assertion Failed: {0}", __VA_ARGS__); MYKA_DEBUGBREAK(); } }
    #define MYKA_ASSERT_NO_MSG(x)        { if(!(x)) { MYKA_ERROR("Assertion Failed: {0}", #x); MYKA_DEBUGBREAK(); } }
    
    #define MYKA_ASSERT(...) MYKA_EXPAND_MACRO( MYKA_GET_ASSERT_MACRO(__VA_ARGS__, MYKA_ASSERT_WITH_MSG, MYKA_ASSERT_NO_MSG)(__VA_ARGS__) )
    
    #define MYKA_CORE_ASSERT_WITH_MSG(x, ...) { if(!(x)) { MYKA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); MYKA_DEBUGBREAK(); } }
    #define MYKA_CORE_ASSERT_NO_MSG(x)        { if(!(x)) { MYKA_CORE_ERROR("Assertion Failed: {0}", #x); MYKA_DEBUGBREAK(); } }
    
    #define MYKA_CORE_ASSERT(...) MYKA_EXPAND_MACRO( MYKA_GET_ASSERT_MACRO(__VA_ARGS__, MYKA_CORE_ASSERT_WITH_MSG, MYKA_CORE_ASSERT_NO_MSG)(__VA_ARGS__) )
#else
    #define MYKA_ASSERT(...)
    #define MYKA_CORE_ASSERT(...)
#endif // End Asserts
