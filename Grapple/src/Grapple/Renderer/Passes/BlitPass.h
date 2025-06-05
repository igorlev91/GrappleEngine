#pragma once

#include "Grapple/Renderer/Texture.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class Grapple_API BlitPass : public RenderGraphPass
	{
	public:
		BlitPass(Ref<Texture> sourceTexture, TextureFiltering filter);
		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;

		static void ConfigureSpecifications(RenderGraphPassSpecifications& specifications, Ref<Texture> source, Ref<Texture> destination);
	private:
		TextureFiltering m_Filter = TextureFiltering::Closest;
		Ref<Texture> m_SourceTexture = nullptr;
	};
}
