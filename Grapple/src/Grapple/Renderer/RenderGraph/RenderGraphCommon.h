#pragma once

#include "GrappleCore/Core.h"

#include "Grapple/Renderer/Texture.h"

#include <stdint.h>

namespace Grapple
{
	struct LayoutTransition
	{
		Ref<Texture> TextureHandle = nullptr;
		ImageLayout InitialLayout = ImageLayout::Undefined;
		ImageLayout FinalLayout = ImageLayout::Undefined;
	};

	struct ExternalRenderGraphResource
	{
		Ref<Texture> TextureHandle = nullptr;
		ImageLayout InitialLayout = ImageLayout::Undefined;
		ImageLayout FinalLayout = ImageLayout::Undefined;
	};

	struct LayoutTransitionsRange
	{
		LayoutTransitionsRange() = default;

		LayoutTransitionsRange(uint32_t startAndEnd)
			: Start(startAndEnd), End(startAndEnd) {}

		LayoutTransitionsRange(uint32_t start, uint32_t end)
			: Start(start), End(end) {}

		uint32_t Start = UINT32_MAX;
		uint32_t End = UINT32_MAX;
	};

	struct Grapple_API CompiledRenderGraph
	{
		void Reset();

		std::vector<LayoutTransition> LayoutTransitions;
		LayoutTransitionsRange ExternalResourceFinalTransitions;
	};
}
