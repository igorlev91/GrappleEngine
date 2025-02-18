#pragma once

#include <Grapple/Core/Platform.h>

#include <memory>

#ifdef Grapple_DEBUG
	#ifdef FL_PLATFORM_WINDOWS
		#define FL_DEBUGBREAK __debugbreak()
	#endif
#endif

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

	template<typename T, typename ...Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}