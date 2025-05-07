#pragma once

#include <memory>
#include <xhash>

#ifdef _WIN32
	#ifdef _WIN64
		#define Grapple_PLATFORM_WINDOWS
	#else
		#error "x86 platform is not supported"
	#endif
#endif

#ifdef Grapple_DEBUG
	#ifdef Grapple_PLATFORM_WINDOWS
		#define Grapple_DEBUGBREAK __debugbreak()
	#else
		#define Grapple_DEBUGBREAK
	#endif
#else
	#define Grapple_DEBUGBREAK
#endif

#define Grapple_NONE

#define Grapple_API_EXPORT __declspec(dllexport)
#define Grapple_API_IMPORT __declspec(dllimport)

#define Grapple_TYPE_OF_FIELD(typeName, fieldName) decltype(((typeName*)nullptr)->fieldName)

#define Grapple_EXPORT extern "C" __declspec(dllexport)
#define Grapple_IMPORT extern "C" __declspec(dllimport)

#define Grapple_EXPEND_MACRO(a) a
#define FALRE_STRINGIFY_MACRO(a) #a

#define Grapple_IMPL_ENUM_BITFIELD(enumName) \
	constexpr enumName operator&(enumName a, enumName b) { return (enumName) ((uint64_t)a & (uint64_t)b); } \
	constexpr enumName operator|(enumName a, enumName b) { return (enumName) ((uint64_t)a | (uint64_t)b); } \
	constexpr enumName operator~(enumName a) { return (enumName) (~(uint64_t)a); } \
	constexpr enumName& operator|=(enumName& a, enumName b) { a = (enumName) ((uint64_t)a | (uint64_t)b); return a; } \
	constexpr enumName& operator&=(enumName& a, enumName b) { a = (enumName) ((uint64_t)a & (uint64_t)b); return a; } \
	constexpr bool operator==(enumName a, int32_t b) { return (int32_t)a == b; } \
	constexpr bool operator!=(enumName a, int32_t b) { return (int32_t)a != b; }

#define HAS_BIT(value, bit) ((value & bit) != 0)

namespace Grapple
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ...Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename F>
	constexpr Ref<T> As(const Ref<F>& ref)
	{
		return std::static_pointer_cast<T>(ref);
	}

	template<typename T, typename ...Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	void CombineHashes(size_t& seed, const T& value)
	{
		std::hash<T> hasher;
		seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}