#pragma once

#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"
#include "Grapple/Renderer/FrameBuffer.h"

#include "Grapple/Renderer/RenderGraph/RenderGraphCommon.h"

namespace Grapple
{
	struct RenderPassNode
	{
		RenderGraphPassSpecifications Specifications;
		Ref<RenderGraphPass> Pass = nullptr;
		Ref<FrameBuffer> RenderTarget = nullptr;
		LayoutTransitionsRange Transitions;
	};
}
