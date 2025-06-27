#pragma once

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class Grapple_API BlitPass : public RenderGraphPass
	{
	public:
		BlitPass(RenderGraphTextureId sourceTexture, RenderGraphTextureId destination, TextureFiltering filter);

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;

		static void ConfigureSpecifications(RenderGraphPassSpecifications& specifications, RenderGraphTextureId source, RenderGraphTextureId destination);
	private:
		TextureFiltering m_Filter = TextureFiltering::Closest;
		RenderGraphTextureId m_SourceTexture;
		RenderGraphTextureId m_Destination;
	};
}
