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
		commandBuffer->Blit(m_SourceTexture, context.GetRenderTarget()->GetAttachment(0), m_Filter);

		Ref<VulkanCommandBuffer> vulkanCommandBuffer = As<VulkanCommandBuffer>(commandBuffer);
		vulkanCommandBuffer->TransitionImageLayout(
			As<VulkanTexture>(m_SourceTexture)->GetImageHandle(),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}
}
