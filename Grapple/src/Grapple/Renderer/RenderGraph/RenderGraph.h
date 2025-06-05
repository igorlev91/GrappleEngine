#pragma once

#include "GrappleCore/Collections/Span.h"

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"
#include "Grapple/Renderer/RenderGraph/RenderPassNode.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphCommon.h"

#include <optional>

namespace Grapple
{
	class FrameBuffer;
	class Grapple_API RenderGraph
	{
	public:
		using ResourceId = uint64_t;

		void AddPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass);

		void AddExternalResource(const ExternalRenderGraphResource& resource);

		void Execute(Ref<CommandBuffer> commandBuffer);
		void Build();
		void Clear();
	private:
		void ExecuteLayoutTransitions(Ref<CommandBuffer> commandBuffer, LayoutTransitionsRange range);
	private:
		std::vector<RenderPassNode> m_Nodes;
		std::vector<ExternalRenderGraphResource> m_ExternalResources;

		CompiledRenderGraph m_CompiledRenderGraph;
	};
}
