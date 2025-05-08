#pragma once

#include "Grapple/Math/Math.h"

#include "Grapple/Renderer/FrameBuffer.h"
#include "Grapple/Renderer/RenderTargetsPool.h"

namespace Grapple
{
	struct RenderingContext
	{
		RenderingContext(const Ref<FrameBuffer>& renderTarget, RenderTargetsPool& rtPool)
			: RenderTarget(renderTarget), RTPool(rtPool) {}

		const Ref<FrameBuffer> RenderTarget;
		RenderTargetsPool& RTPool;
	};
}