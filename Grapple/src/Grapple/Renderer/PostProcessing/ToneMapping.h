#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore//Serialization/SerializationStream.h"

#include "Grapple/Renderer/PostProcessing/PostProcessingEffect.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class Material;

	class Grapple_API ToneMapping : public PostProcessingEffect
	{
	public:
		Grapple_TYPE;

		ToneMapping();

		void RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport) override;
		const SerializableObjectDescriptor& GetSerializationDescriptor() const override;
	};

	template<>
	struct TypeSerializer<ToneMapping>
	{
		void OnSerialize(ToneMapping& toneMapping, SerializationStream& stream)
		{
		}
	};



	class Grapple_API ToneMappingPass : public RenderGraphPass
	{
	public:
		ToneMappingPass(RenderGraphTextureId colorTexture);

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
	private:
		Ref<Material> m_Material = nullptr;
		RenderGraphTextureId m_ColorTexture;
	};
}