#include "BlitPass.h"

#include "GrappleCore/Profiler/Profiler.h"

#include "Grapple/Renderer/CommandBuffer.h"
#include "Grapple/Renderer/FrameBuffer.h"

namespace Grapple
{
	BlitPass::BlitPass(Ref<Texture> sourceTexture, Ref<Texture> destination, TextureFiltering filter)
		: m_SourceTexture(sourceTexture), m_Destination(destination), m_Filter(filter)
	{
	}

	void BlitPass::OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer)
	{
		Grapple_PROFILE_FUNCTION();
		commandBuffer->Blit(m_SourceTexture, m_Destination, m_Filter);
	}

	void BlitPass::ConfigureSpecifications(RenderGraphPassSpecifications& specifications, Ref<Texture> source, Ref<Texture> destination)
	{
		Grapple_PROFILE_FUNCTION();
		specifications.SetType(RenderGraphPassType::Other);
		specifications.AddInput(source, ImageLayout::TransferSource);
		specifications.AddOutput(destination, 0, ImageLayout::TransferDestination);
	}
}
