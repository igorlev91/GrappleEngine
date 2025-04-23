#pragma once

#include "Grapple/Renderer/RenderingContext.h"

namespace Grapple
{
	class Grapple_API RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		virtual void OnRender(RenderingContext& context) = 0;
	};
}