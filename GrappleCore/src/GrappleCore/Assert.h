#pragma once

#include "GrappleCore/Log.h"
#include "GrappleCore/Core.h"

#include <filesystem>

#if defined(Grapple_DEBUG) || defined(Grapple_RELEASE)
	#define Grapple_ENABLE_ASSERTIONS
#endif

#ifdef Grapple_ENABLE_ASSERTIONS
	#define Grapple_ASSERT_IMPL(type, condition, msg, ...) { if (!(condition)) { Grapple##type##ERROR(msg, __VA_ARGS__); Grapple_DEBUGBREAK; } }
	#define Grapple_ASSERT_IMPL_WITH_MSG(type, condition, ...) Grapple_EXPEND_MACRO(Grapple_ASSERT_IMPL(type, condition, "Assertion failed: {0}", __VA_ARGS__))
	#define Grapple_ASSERT_IMPL_WITHOUT_MSG(type, condition, ...) Grapple_EXPEND_MACRO(Grapple_ASSERT_IMPL(type, condition, "Assertion '{0}' failed at {1}:{2}", FALRE_STRINGIFY_MACRO(condition), std::filesystem::path(__FILE__).filename().string(), __LINE__))

	#define Grapple_GET_ASSERT_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define Grapple_GET_ASSERT_MACRO(...) Grapple_EXPEND_MACRO(Grapple_GET_ASSERT_MACRO_NAME(__VA_ARGS__, Grapple_ASSERT_IMPL_WITH_MSG, Grapple_ASSERT_IMPL_WITHOUT_MSG))

	#define Grapple_CORE_ASSERT(...) Grapple_EXPEND_MACRO(Grapple_GET_ASSERT_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__));
	#define Grapple_ASSERT(...) Grapple_EXPEND_MACRO(Grapple_GET_ASSERT_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#else
	#define Grapple_CORE_ASSERT(...)
	#define Grapple_ASSERT(...)
#endif