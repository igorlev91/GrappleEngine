#include "BlitPass.h"

#include "Grapple/Renderer/CommandBuffer.h"
#include "Grapple/Renderer/FrameBuffer.h"

#include "Grapple/Platform/Vulkan/VulkanCommandBuffer.h"

namespace Grapple
{
	BlitPass::BlitPass(Ref<Texture> sourceTexture, TextureFiltering filter)
		: m_SourceTexture(sourceTexture), m_Filter(filter)
	{
	}

	void BlitPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);

#if 0
		// NOTE: Blit transitions the source texture from COLOR_ATTACHMENT_OUTPUT_OPTIMAL to TRANSFER_SRC
		vulkanCommandBuffer->TransitionImageLayout(
			As<VulkanTexture>(m_SourceTexture)->GetImageHandle(),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
#endif

		commandBuffer->Blit(m_SourceTexture, context.GetRenderTarget()->GetAttachment(0), m_Filter);

#if 0
		vulkanCommandBuffer->TransitionImageLayout(
			As<VulkanTexture>(m_SourceTexture)->GetImageHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
#endif
	}

	void BlitPass::ConfigureSpecifications(RenderGraphPassSpecifications& specifications, Ref<Texture> source, Ref<Texture> destination)
	{
		specifications.AddInput(source, ImageLayout::TransferSource);
		specifications.AddOutput(destination, 0, ImageLayout::TransferDestination);
	}
}
