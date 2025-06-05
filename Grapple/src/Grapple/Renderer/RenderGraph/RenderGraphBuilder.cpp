#include "RenderGraphBuilder.h"

#include "GrappleCore/Assert.h"
#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	RenderGraphBuilder::RenderGraphBuilder(CompiledRenderGraph& result, Span<RenderPassNode> nodes, Span<ExternalRenderGraphResource> externalResources)
		: m_Result(result), m_Nodes(nodes), m_ExternalResources(externalResources)
	{
	}

	void RenderGraphBuilder::Build()
	{
		Grapple_PROFILE_FUNCTION();

		// Setup initial state for external resources
		for (const auto& resource : m_ExternalResources)
		{
			if (resource.InitialLayout == ImageLayout::Undefined)
				continue;

			ResourceState& state = m_States[GetResoureId(resource.TextureHandle)];
			state.Layout = resource.InitialLayout;
		}

		m_RenderPassTransitions.reserve(m_Nodes.GetSize());

		for (size_t nodeIndex = 0; nodeIndex < m_Nodes.GetSize(); nodeIndex++)
		{
			m_Nodes[nodeIndex].Transitions = LayoutTransitionsRange((uint32_t)m_Result.LayoutTransitions.size());
			GenerateInputTransitions(nodeIndex);
			GenerateOutputTransitions(nodeIndex);
		}

		m_Result.ExternalResourceFinalTransitions = LayoutTransitionsRange((uint32_t)m_Result.LayoutTransitions.size());
		for (const auto& resource : m_ExternalResources)
		{
			AddTransition(resource.TextureHandle, resource.FinalLayout, m_Result.ExternalResourceFinalTransitions);
		}

		CreateRenderTargets();
	}

	void RenderGraphBuilder::GenerateInputTransitions(size_t nodeIndex)
	{
		Grapple_PROFILE_FUNCTION();
		RenderPassNode& node = m_Nodes[nodeIndex];

		m_RenderPassTransitions.emplace_back().resize(node.Specifications.GetOutputs().size());

		for (const auto& input : node.Specifications.GetInputs())
		{
			AddTransition(input.InputTexture, input.Layout, node.Transitions);
		}
	}

	void RenderGraphBuilder::GenerateOutputTransitions(size_t nodeIndex)
	{
		Grapple_PROFILE_FUNCTION();
		RenderPassNode& node = m_Nodes[nodeIndex];
		const auto& outputs = node.Specifications.GetOutputs();
		for (size_t outputIndex = 0; outputIndex < node.Specifications.GetOutputs().size(); outputIndex++)
		{
			const auto& output = outputs[outputIndex];

			Grapple_CORE_ASSERT(output.AttachmentTexture != nullptr);
			ResourceId id = GetResoureId(output.AttachmentTexture);
			auto it = m_States.find(id);

			// Only outputs with AttachmentOutput image layout can be used in a RenderPass
			if (output.Layout == ImageLayout::AttachmentOutput)
			{
				LayoutTransition& transition = m_RenderPassTransitions[nodeIndex][outputIndex];
				transition.TextureHandle = output.AttachmentTexture;
				transition.InitialLayout = GetCurrentLayout(output.AttachmentTexture);
				transition.FinalLayout = output.Layout;

				ResourceState& state = m_States[id];
				state.Layout = output.Layout;
				state.LastWritingPass = WritingRenderPass{};
				state.LastWritingPass->RenderPassIndex = (uint32_t)nodeIndex;
				state.LastWritingPass->AttachmentIndex = (uint32_t)outputIndex;
			}
			else
			{
				AddExplicitTransition(output.AttachmentTexture, output.Layout, node.Transitions);
			}

			outputIndex++;
		}
	}

	void RenderGraphBuilder::CreateRenderTargets()
	{
		Grapple_PROFILE_FUNCTION();

		std::vector<VkAttachmentDescription> attachmentDescriptions;
		std::vector<Ref<Texture>> attachmentTextures;

		for (size_t nodeIndex = 0; nodeIndex < m_Nodes.GetSize(); nodeIndex++)
		{
			RenderPassNode& node = m_Nodes[nodeIndex];

			// RenderTargets are only created for Graphics render passes
			if (node.Specifications.GetType() != RenderGraphPassType::Graphics)
				continue;

			attachmentDescriptions.clear();
			attachmentTextures.clear();

			std::optional<uint32_t> depthAttachmentIndex = {};

			const auto& outputs = m_Nodes[nodeIndex].Specifications.GetOutputs();
			for (size_t outputIndex = 0; outputIndex < outputs.size(); outputIndex++)
			{
				VkAttachmentDescription& description = attachmentDescriptions.emplace_back();
				const LayoutTransition& transition = m_RenderPassTransitions[nodeIndex][outputIndex];
				TextureFormat format = outputs[outputIndex].AttachmentTexture->GetFormat();

				Grapple_CORE_ASSERT(transition.TextureHandle != nullptr);

				if (IsDepthTextureFormat(format))
				{
					Grapple_CORE_ASSERT(!depthAttachmentIndex);
					depthAttachmentIndex = (uint32_t)outputIndex;
				}

				description.format = TextureFormatToVulkanFormat(format);
				description.flags = 0;
				description.initialLayout = ImageLayoutToVulkanImageLayout(transition.InitialLayout, format);
				description.finalLayout = ImageLayoutToVulkanImageLayout(transition.FinalLayout, format);
				description.samples = VK_SAMPLE_COUNT_1_BIT;
				description.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				if (description.initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
				{
					description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				}

				Grapple_CORE_ASSERT(description.finalLayout != VK_IMAGE_LAYOUT_UNDEFINED);

				attachmentTextures.push_back(outputs[outputIndex].AttachmentTexture);
			}

			Ref<VulkanRenderPass> compatibleRenderPass = CreateRef<VulkanRenderPass>(
				Span<VkAttachmentDescription>::FromVector(attachmentDescriptions),
				depthAttachmentIndex);

			node.RenderTarget = CreateRef<VulkanFrameBuffer>(
				attachmentTextures[0]->GetWidth(),
				attachmentTextures[0]->GetHeight(),
				compatibleRenderPass,
				Span<Ref<Texture>>::FromVector(attachmentTextures),
				false);

			node.RenderTarget->SetDebugName(node.Specifications.GetDebugName());
		}
	}

	void RenderGraphBuilder::AddExplicitTransition(Ref<Texture> texture, ImageLayout layout, LayoutTransitionsRange& transitions)
	{
		Grapple_PROFILE_FUNCTION();

		Grapple_CORE_ASSERT(texture);
		Grapple_CORE_ASSERT(layout != ImageLayout::Undefined);

		ResourceId id = GetResoureId(texture);
		ResourceStateIterator it = m_States.find(id);

		ImageLayout initialLayout = ImageLayout::Undefined;

		if (it != m_States.end())
		{
			initialLayout = it->second.Layout;
		}

		if (initialLayout == layout)
			return;

		transitions.End++;
		auto& transition = m_Result.LayoutTransitions.emplace_back();
		transition.TextureHandle = texture;
		transition.InitialLayout = initialLayout;
		transition.FinalLayout = layout;

		m_States[id] = { layout, {} };
	}

	void RenderGraphBuilder::AddTransition(Ref<Texture> texture, ImageLayout layout, LayoutTransitionsRange& transitions)
	{
		Grapple_PROFILE_FUNCTION();

		Grapple_CORE_ASSERT(texture);
		Grapple_CORE_ASSERT(layout != ImageLayout::Undefined);

		ResourceId id = GetResoureId(texture);
		auto it = m_States.find(id);

		bool explicitTransition = false;
		if (it == m_States.end())
		{
			explicitTransition = true;
		}
		else
		{
			ResourceState& state = it->second;

			if (state.LastWritingPass)
			{
				auto lastRenderPass = state.LastWritingPass;
				m_RenderPassTransitions[lastRenderPass->RenderPassIndex][lastRenderPass->AttachmentIndex].FinalLayout = layout;

				state.Layout = layout;
				state.LastWritingPass = {};
			}
			else
			{
				explicitTransition = true;
			}
		}

		if (explicitTransition)
		{
			AddExplicitTransition(texture, layout, transitions);
		}
	}

	ImageLayout RenderGraphBuilder::GetCurrentLayout(Ref<Texture> texture)
	{
		Grapple_CORE_ASSERT(texture != nullptr);

		auto it = m_States.find(GetResoureId(texture));
		if (it == m_States.end())
		{
			return ImageLayout::Undefined;
		}

		return it->second.Layout;
	}
}
