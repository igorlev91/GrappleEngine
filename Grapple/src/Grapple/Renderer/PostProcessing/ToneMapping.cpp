#include "Tonemapping.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer/ShaderLibrary.h"

#include "Grapple/Renderer/CommandBuffer.h"
#include "Grapple/Renderer/GraphicsContext.h"
#include "Grapple/Renderer/RendererPrimitives.h"

#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"

#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(ToneMapping);

	ToneMapping::ToneMapping()
		: RenderPass(RenderPassQueue::PostProcessing), Enabled(false)
	{
		std::optional<AssetHandle> shaderHandle = ShaderLibrary::FindShader("AcesToneMapping");
		if (!shaderHandle || !AssetManager::IsAssetHandleValid(shaderHandle.value()))
		{
			Grapple_CORE_ERROR("ToneMapping: Failed to find ToneMapping shader");
			return;
		}

		Ref<Shader> shader = AssetManager::GetAsset<Shader>(shaderHandle.value());
		m_Material = Material::Create(shader);

		m_ColorTexture = shader->GetPropertyIndex("u_ScreenBuffer");
	}

	void ToneMapping::OnRender(RenderingContext& context)
	{
		Grapple_PROFILE_FUNCTION();

		if (!Enabled || !Renderer::GetCurrentViewport().PostProcessingEnabled)
			return;

		FrameBufferTextureFormat formats[] = { FrameBufferTextureFormat::RGB8 };

		Ref<FrameBuffer> output = context.RTPool.GetFullscreen(Span(formats, 1));

		if (m_ColorTexture)
		{
			m_Material->GetPropertyValue<TexturePropertyValue>(*m_ColorTexture).SetFrameBuffer(context.RenderTarget, 0);
		}

		Ref<CommandBuffer> commandBuffer = GraphicsContext::GetInstance().GetCommandBuffer();
		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);
			vulkanCommandBuffer->TransitionImageLayout(
				As<VulkanFrameBuffer>(output)->GetAttachmentImage(0),
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			vulkanCommandBuffer->TransitionImageLayout(
				As<VulkanFrameBuffer>(context.RenderTarget)->GetAttachmentImage(0),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		commandBuffer->BeginRenderTarget(output);

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);
			vulkanCommandBuffer->SetPrimaryDescriptorSet(nullptr);
			vulkanCommandBuffer->SetSecondaryDescriptorSet(nullptr);
		}

		commandBuffer->ApplyMaterial(m_Material);

		const auto& frameBufferSpec = context.RenderTarget->GetSpecifications();
		commandBuffer->SetViewportAndScisors(Math::Rect(0.0f, 0.0f, (float)frameBufferSpec.Width, (float)frameBufferSpec.Height));

		commandBuffer->DrawIndexed(RendererPrimitives::GetFullscreenQuadMesh(), 0, 0, 1);
		commandBuffer->EndRenderTarget();

		commandBuffer->Blit(output, 0, context.RenderTarget, 0, TextureFiltering::Closest);

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);

			vulkanCommandBuffer->TransitionImageLayout(
				As<VulkanFrameBuffer>(output)->GetAttachmentImage(0),
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}

		context.RTPool.ReturnFullscreen(Span(formats, 1), output);
	}
}
