#include "TextPass.h"

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/Font.h"

#include "Grapple/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanPipeline.h"

namespace Grapple
{
	TextPass::TextPass(const Renderer2DFrameData& frameData, const Renderer2DLimits& limits, Ref<IndexBuffer> indexBuffer, Ref<Shader> textShader)
		: m_FrameData(frameData), m_RendererLimits(limits), m_IndexBuffer(indexBuffer), m_TextShader(textShader)
	{
		m_VertexBuffer = VertexBuffer::Create(sizeof(TextVertex) * 4 * m_RendererLimits.MaxQuadCount, GPUBufferUsage::Static);
		As<VulkanVertexBuffer>(m_VertexBuffer)->GetBuffer().EnsureAllocated(); // HACk: To avoid binding NULL buffer
	}

	TextPass::~TextPass()
	{
		Grapple_PROFILE_FUNCTION();
		ReleaseDescriptorSets();
	}

	void TextPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();
		ReleaseDescriptorSets();

		if (m_FrameData.TextQuadCount > 0)
		{
			m_VertexBuffer->SetData(MemorySpan(const_cast<TextVertex*>(m_FrameData.TextVertices.data()), m_FrameData.TextQuadCount * 4), 0, commandBuffer);
		}

		Ref<FrameBuffer> renderTarget = context.GetRenderTarget();

		if (m_TextPipeline == nullptr)
		{
			PipelineSpecifications specificaionts{};
			specificaionts.Shader = m_TextShader;
			specificaionts.Culling = CullingMode::Back;
			specificaionts.DepthTest = false;
			specificaionts.DepthWrite = false;
			specificaionts.InputLayout = PipelineInputLayout({
				{ 0, 0, ShaderDataType::Float3 }, // Position
				{ 0, 1, ShaderDataType::Float4 }, // COlor
				{ 0, 2, ShaderDataType::Float2 }, // UV
				{ 0, 3, ShaderDataType::Int }, // Entity index
			});

			Ref<const DescriptorSetLayout> layouts[] =
			{
				Renderer::GetCameraDescriptorSetPool()->GetLayout(),
				m_FrameData.TextDescriptorSetsPool->GetLayout()
			};

			m_TextPipeline = CreateRef<VulkanPipeline>(specificaionts,
				As<VulkanFrameBuffer>(renderTarget)->GetCompatibleRenderPass(),
				Span<Ref<const DescriptorSetLayout>>(layouts, 2),
				Span<ShaderPushConstantsRange>());
		}

		commandBuffer->BeginRenderTarget(context.GetRenderTarget());
		commandBuffer->SetViewportAndScisors(Math::Rect(glm::vec2(0.0f), (glm::vec2)renderTarget->GetSize()));

		commandBuffer->SetGlobalDescriptorSet(context.GetViewport().GlobalResources.CameraDescriptorSet, 0);
		
		for (const auto& batch : m_FrameData.TextBatches)
		{
			if (batch.Count == 0)
				continue;

			FlushBatch(batch, commandBuffer);
		}

		commandBuffer->EndRenderTarget();
	}

	void TextPass::FlushBatch(const TextBatch& batch, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();
		Ref<DescriptorSet> set = m_FrameData.TextDescriptorSetsPool->AllocateSet();
		set->SetDebugName("TextDescriptorSet");
		set->WriteImage(batch.Font->GetAtlas(), 0);
		set->FlushWrites();

		m_UsedSets.push_back(set);

		commandBuffer->SetGlobalDescriptorSet(set, 1);
		commandBuffer->BindPipeline(m_TextPipeline);

		commandBuffer->BindVertexBuffers(Span((Ref<const VertexBuffer>)m_VertexBuffer), 0);
		commandBuffer->BindIndexBuffer(m_IndexBuffer);
		commandBuffer->DrawIndexed(batch.Start * 6, batch.Count * 6, 0, 0, 1);
	}

	void TextPass::ReleaseDescriptorSets()
	{
		Grapple_PROFILE_FUNCTION();
		for (const auto& set : m_UsedSets)
		{
			m_FrameData.TextDescriptorSetsPool->ReleaseSet(set);
		}

		m_UsedSets.clear();
	}
}
