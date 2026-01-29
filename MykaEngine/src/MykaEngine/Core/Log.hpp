#pragma once

#include "Base.hpp"
#include "spdlog/spdlog.h"

namespace Myka
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger> &GetCoreLogger() { return m_CoreLogger; }
		static Ref<spdlog::logger> &GetClientLogger() { return m_ClientLogger; }

	private:
		static Ref<spdlog::logger> m_CoreLogger;
		static Ref<spdlog::logger> m_ClientLogger;
	};
} // namespace Myka

// Core log macros
#define MYKA_CORE_TRACE(...) 	::Myka::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MYKA_CORE_INFO(...) 	::Myka::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MYKA_CORE_WARN(...) 	::Myka::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MYKA_CORE_ERROR(...) 	::Myka::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MYKA_CORE_CRITICAL(...) ::Myka::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MYKA_TRACE(...) 		::Myka::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MYKA_INFO(...) 			::Myka::Log::GetClientLogger()->info(__VA_ARGS__)
#define MYKA_WARN(...) 			::Myka::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MYKA_ERROR(...) 		::Myka::Log::GetClientLogger()->error(__VA_ARGS__)
#define MYKA_CRITICAL(...) 		::Myka::Log::GetClientLogger()->critical(__VA_ARGS__)
