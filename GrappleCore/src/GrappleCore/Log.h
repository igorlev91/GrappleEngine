#pragma once

#include "GrappleCore/Core.h"

#include <spdlog/spdlog.h>

namespace Grapple
{
	class GrappleCORE_API Log
	{
	public:
		static void Initialize();
		
		static Ref<spdlog::logger> GetCoreLogger();
		static Ref<spdlog::logger> GetClientLogger();
	};
}

#define Grapple_CORE_ERROR(...) Grapple::Log::GetCoreLogger()->error(__VA_ARGS__)
#define Grapple_CORE_WARN(...) Grapple::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define Grapple_CORE_INFO(...) Grapple::Log::GetCoreLogger()->info(__VA_ARGS__)
#define Grapple_CORE_TRACE(...) Grapple::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define Grapple_CORE_CRITICAL(...) Grapple::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define Grapple_ERROR(...) Grapple::Log::GetClientLogger()->error(__VA_ARGS__)
#define Grapple_WARN(...) Grapple::Log::GetClientLogger()->warn(__VA_ARGS__)
#define Grapple_INFO(...) Grapple::Log::GetClientLogger()->info(__VA_ARGS__)
#define Grapple_TRACE(...) Grapple::Log::GetClientLogger()->trace(__VA_ARGS__)
#define Grapple_CRITICAL(...) Grapple::Log::GetClientLogger()->critical(__VA_ARGS__)
