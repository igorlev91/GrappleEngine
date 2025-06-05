#pragma once

#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"
#include "Grapple/Renderer/FrameBuffer.h"

namespace Grapple
{
	struct LayoutTransition
	{
		Ref<Texture> TextureHandle = nullptr;
		ImageLayout InitialLayout = ImageLayout::Undefined;
		ImageLayout FinalLayout = ImageLayout::Undefined;
	};

	struct RenderPassNode
	{
		RenderGraphPassSpecifications Specifications;
		Ref<RenderGraphPass> Pass = nullptr;
		Ref<FrameBuffer> RenderTarget = nullptr;
		std::vector<LayoutTransition> Transitions;
	};
}
