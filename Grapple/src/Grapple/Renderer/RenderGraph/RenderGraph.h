#pragma once

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"
#include "Grapple/Renderer/RenderGraph/RenderPassNode.h"

#include <optional>

namespace Grapple
{
	class FrameBuffer;
	class Grapple_API RenderGraph
	{
	public:
		using ResourceId = uint64_t;

		void AddPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass);

		// Adds a layout transitions for a given texture.
		// The transition is performed after finishing all the render passes in the graph.
		// Initial layout is infered based on the inputs & outputs of render passes in the graph.
		void AddFinalTransition(Ref<Texture> texture, ImageLayout finalLayout);

		void Execute(Ref<CommandBuffer> commandBuffer);
		void Build();
		void Clear();
	private:
		void ExecuteLayoutTransitions(Ref<CommandBuffer> commandBuffer, const std::vector<LayoutTransition>& transitions);
	private:
		std::vector<RenderPassNode> m_Nodes;
		std::vector<LayoutTransition> m_FinalTransitions;
	};
}
