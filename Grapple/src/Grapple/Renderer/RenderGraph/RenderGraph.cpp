#include "RenderGraph.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphBuilder.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"

namespace Grapple
{
	RenderGraph::RenderGraph(const Viewport& viewport)
		: m_Viewport(viewport), m_ResourceManager(viewport)
	{
	}

	void RenderGraph::AddPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass)
	{
		Grapple_CORE_ASSERT(pass != nullptr);

		auto& node = m_Nodes.emplace_back();
		node.Pass = pass;
		node.Specifications = specifications;
	}

	void RenderGraph::InsertPass(const RenderGraphPassSpecifications& specifications, Ref<RenderGraphPass> pass, size_t index)
	{
		Grapple_CORE_ASSERT(pass != nullptr);
		Grapple_CORE_ASSERT(index <= m_Nodes.size());

		m_Nodes.insert(m_Nodes.begin() + index, RenderPassNode{ specifications, pass, nullptr, {} });
	}

	const RenderPassNode* RenderGraph::GetRenderPassNode(size_t index) const
	{
		if (index < m_Nodes.size())
			return &m_Nodes[index];

		return nullptr;
	}

	std::optional<size_t> RenderGraph::FindPassByName(std::string_view name) const
	{
		Grapple_PROFILE_FUNCTION();
		for (size_t i = 0; i < m_Nodes.size(); i++)
		{
			if (m_Nodes[i].Specifications.GetDebugName() == name)
			{
				return i;
			}
		}

		return {};
	}

	void RenderGraph::AddExternalResource(const ExternalRenderGraphResource& resource)
	{
		Grapple_CORE_ASSERT(resource.FinalLayout != ImageLayout::Undefined);
		m_ExternalResources.push_back(resource);
	}

	void RenderGraph::Execute(Ref<CommandBuffer> commandBuffer, const SceneSubmition& sceneSubmition, const RenderView& view)
	{
		Grapple_PROFILE_FUNCTION();
		Grapple_CORE_ASSERT(m_IsValid);

		Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);

		for (const auto& node : m_Nodes)
		{
			RenderGraphContext context(m_Viewport, node.RenderTarget, *this, m_ResourceManager, sceneSubmition, view);

			ExecuteLayoutTransitions(commandBuffer, node.Transitions);

			node.Pass->OnRender(context, commandBuffer);
		}

		ExecuteLayoutTransitions(commandBuffer, m_CompiledRenderGraph.ExternalResourceFinalTransitions);
	}

	void RenderGraph::Build()
	{
		Grapple_PROFILE_FUNCTION();
		RenderGraphBuilder builder(m_CompiledRenderGraph,
			Span<RenderPassNode>::FromVector(m_Nodes),
			m_ResourceManager,
			Span<ExternalRenderGraphResource>::FromVector(m_ExternalResources));

		builder.Build();

		m_NeedsRebuilding = false;
		m_IsValid = true;
	}

	void RenderGraph::Clear()
	{
		Grapple_PROFILE_FUNCTION();
		m_Nodes.clear();
		m_CompiledRenderGraph.Reset();
		m_ResourceManager.Clear();

		m_IsValid = false;
	}

	void RenderGraph::OnViewportResize()
	{
		Grapple_PROFILE_FUNCTION();

		m_ResourceManager.ResizeTextures();
		CreateRenderTargets();
	}

	void RenderGraph::CreateRenderTargets()
	{
		Grapple_PROFILE_FUNCTION();
		Grapple_CORE_ASSERT(m_IsValid);

		std::vector<Ref<Texture>> attachmentTextures;
		for (RenderPassNode& node : m_Nodes)
		{
			if (node.RenderTarget == nullptr)
				continue;

			attachmentTextures.clear();

			for (const auto& output : node.Specifications.GetOutputs())
			{
				attachmentTextures.push_back(m_ResourceManager.GetTexture(output.AttachmentTexture));
			}

			Ref<VulkanRenderPass> compatibleRenderPass = As<VulkanFrameBuffer>(node.RenderTarget)->GetCompatibleRenderPass();

			std::string debugName = node.RenderTarget->GetDebugName();

			node.RenderTarget = CreateRef<VulkanFrameBuffer>(attachmentTextures[0]->GetWidth(),
				attachmentTextures[0]->GetHeight(),
				compatibleRenderPass,
				Span<Ref<Texture>>::FromVector(attachmentTextures),
				false);
			node.RenderTarget->SetDebugName(debugName);
		}
	}

	void RenderGraph::ExecuteLayoutTransitions(Ref<CommandBuffer> commandBuffer, LayoutTransitionsRange range)
	{
		Grapple_PROFILE_FUNCTION();
		Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);

		for (uint32_t i = range.Start; i < range.End; i++)
		{
			const LayoutTransition& transition = m_CompiledRenderGraph.LayoutTransitions[i];

			Ref<Texture> texture = m_ResourceManager.GetTexture(transition.Texture);
			VkImage image = As<VulkanTexture>(texture)->GetImageHandle();

			TextureFormat format = texture->GetFormat();
			VkImageLayout initialLayout = ImageLayoutToVulkanImageLayout(transition.InitialLayout, format);
			VkImageLayout finalLayout = ImageLayoutToVulkanImageLayout(transition.FinalLayout, format);

			if (IsDepthTextureFormat(format))
			{
				vulkanCommandBuffer->TransitionDepthImageLayout(image, HasStencilComponent(format), initialLayout, finalLayout);
			}
			else
			{
				vulkanCommandBuffer->TransitionImageLayout(image, initialLayout, finalLayout);
			}
		}
	}
}
