#pragma once

#include "GrappleCore/Core.h"

#include <stdint.h>

namespace Grapple
{
	class Grapple_API RendererAPI
	{
	public:
		enum class API
		{
			None,
			Vulkan,
		};
	public:
		static void Create(API api);

		static API GetAPI();
	};
}