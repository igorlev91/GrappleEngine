#pragma once

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class FrameBuffer;
	class VulkanRenderPass;
	class Grapple_API RenderGraph
	{
	public:
		using ResourceId = uint64_t;

		void AddPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass);

		void Execute(Ref<CommandBuffer> commandBuffer);
		void Build();
		void Clear();

		void Build2();
	private:
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
			std::vector<LayoutTransition> InputTransitions;
		};

		std::vector<RenderPassNode> m_Nodes;
	};
}
