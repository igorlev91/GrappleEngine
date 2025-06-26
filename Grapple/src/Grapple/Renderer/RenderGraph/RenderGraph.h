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
		void AddPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass);
		void InsertPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass, size_t index);

		const RenderPassNode* GetRenderPassNode(size_t index) const;
		std::optional<size_t> FindPassByName(std::string_view name) const;

		void AddExternalResource(const ExternalRenderGraphResource& resource);

		void Execute(Ref<CommandBuffer> commandBuffer);
		void Build();
		void Clear();

		inline bool NeedsRebuilding() const { return m_NeedsRebuilding; }
		inline void SetNeedsRebuilding() { m_NeedsRebuilding = true; }
	private:
		void ExecuteLayoutTransitions(Ref<CommandBuffer> commandBuffer, LayoutTransitionsRange range);
	private:
		std::vector<RenderPassNode> m_Nodes;
		std::vector<ExternalRenderGraphResource> m_ExternalResources;

		CompiledRenderGraph m_CompiledRenderGraph;

		bool m_NeedsRebuilding = false;
	};
}
