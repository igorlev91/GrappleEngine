#pragma once

#include "Grapple/Core/Core.h"
#include "Grapple/Renderer/RenderData.h"

#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API Renderer
	{
	public:
		static void SetMainViewportSize(glm::uvec2 size);
		static glm::uvec2 GetMainViewportSize();
	};
}