#pragma once

#include "Grapple/Math/Math.h"

#include "Grapple/Renderer/FrameBuffer.h"

namespace Grapple
{
	struct RenderingContext
	{
		RenderingContext(const Ref<FrameBuffer>& renderTarget)
			: RenderTarget(renderTarget) {}

		const Ref<FrameBuffer> RenderTarget;
	};
}