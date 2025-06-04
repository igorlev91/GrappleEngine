#pragma once

#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class FrameBuffer;
	class VulkanRenderPass;

	class Grapple_API RenderGraph
	{
	public:
		void AddPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass);

		void Execute(Ref<CommandBuffer> commandBuffer);
		void Build();
		void Clear();
	private:
		struct RenderPassNode
		{
			RenderGraphPassSpecifications Specifications;
			Ref<RenderGraphPass> Pass = nullptr;
			Ref<FrameBuffer> RenderTarget = nullptr;
		};

		std::vector<RenderPassNode> m_Nodes;
	};
}
