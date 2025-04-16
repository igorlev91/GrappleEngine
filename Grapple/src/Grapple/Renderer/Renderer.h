#pragma once

#include "GrappleCore/Core.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Viewport.h"

#include <glm/glm.hpp>

namespace Grapple
{
	class Grapple_API Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void SetMainViewport(Viewport& viewport);
		static void SetCurrentViewport(Viewport& viewport);

		static Viewport& GetMainViewport();
		static Viewport& GetCurrentViewport();
	};
}