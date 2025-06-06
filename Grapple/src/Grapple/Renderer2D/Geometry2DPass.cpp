#include "Geometry2DPass.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Math/Math.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Renderer/CommandBuffer.h"

#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"

namespace Grapple
{
	Geometry2DPass::Geometry2DPass(const Renderer2DFrameData& frameData, const Renderer2DLimits& limits, Ref<IndexBuffer> indexBuffer, Ref<Material> defaultMaterial)
		: m_FrameData(frameData), m_RendererLimits(limits), m_IndexBuffer(indexBuffer), m_DefaultMaterial(defaultMaterial)
	{
		m_VertexBuffer = VertexBuffer::Create(sizeof(QuadVertex) * 4 * m_RendererLimits.MaxQuadCount, GPUBufferUsage::Static);
	}

	Geometry2DPass::~Geometry2DPass()
	{
		ReleaseDescriptorSets();
	}

	void Geometry2DPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();
		m_VertexBuffer->SetData(MemorySpan::FromVector(const_cast<std::vector<QuadVertex>&>(m_FrameData.QuadVertices)), 0, commandBuffer);

		ReleaseDescriptorSets();

		commandBuffer->BeginRenderTarget(context.GetRenderTarget());

		Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);

		vulkanCommandBuffer->BindVertexBuffers(Span((Ref<const VertexBuffer>*)&m_VertexBuffer, 1));
		vulkanCommandBuffer->BindIndexBuffer(m_IndexBuffer);

		for (const auto& batch : m_FrameData.QuadBatches)
		{
			if (batch.Count == 0)
				continue;

			FlushBatch(batch, commandBuffer);
		}

		commandBuffer->EndRenderTarget();
	}

	void Geometry2DPass::ReleaseDescriptorSets()
	{
		Grapple_PROFILE_FUNCTION();
		for (auto set : m_UsedSets)
		{
			m_FrameData.QuadDescriptorSetsPool->ReleaseSet(set);
		}

		m_UsedSets.clear();
	}

	void Geometry2DPass::FlushBatch(const QuadsBatch& batch, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);
			Ref<VulkanFrameBuffer> renderTarget = As<VulkanFrameBuffer>(Renderer::GetCurrentViewport().RenderTarget);

			Ref<DescriptorSet> descriptorSet = m_FrameData.QuadDescriptorSetsPool->AllocateSet();
			descriptorSet->SetDebugName("QuadsDescriptorSet");

			m_UsedSets.push_back(descriptorSet);

			descriptorSet->WriteImages(Span((Ref<const Texture>*)batch.Textures, Renderer2DLimits::MaxTexturesCount), 0, 0);
			descriptorSet->FlushWrites();

			vulkanCommandBuffer->SetPrimaryDescriptorSet(Renderer::GetPrimaryDescriptorSet());
			vulkanCommandBuffer->SetSecondaryDescriptorSet(descriptorSet);

			vulkanCommandBuffer->ApplyMaterial(batch.Material);
			vulkanCommandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f), (glm::vec2)renderTarget->GetSize()));
			vulkanCommandBuffer->DrawIndexed(batch.Start * 6, batch.Count * 6);
		}
	}
}
