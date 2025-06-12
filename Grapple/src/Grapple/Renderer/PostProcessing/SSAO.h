#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore//Serialization/SerializationStream.h"

#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"
#include "Grapple/Renderer/PostProcessing/PostProcessingEffect.h"

namespace Grapple
{
	class Material;

	class Grapple_API SSAO : public PostProcessingEffect
	{
	public:
		Grapple_TYPE;

		SSAO();

		void RegisterRenderPasses(RenderGraph& renderGraph, const Viewport& viewport) override;
		const SerializableObjectDescriptor& GetSerializationDescriptor() const override;
	public:
		float Bias;
		float Radius;
		float BlurSize;
	};

	template<>
	struct TypeSerializer<SSAO>
	{
		void OnSerialize(SSAO& ssao, SerializationStream& stream)
		{
			stream.Serialize("Radius", SerializationValue(ssao.Radius));
			stream.Serialize("Bias", SerializationValue(ssao.Bias));
			stream.Serialize("BlurSize", SerializationValue(ssao.BlurSize));
		}
	};

	class Grapple_API SSAOMainPass : public RenderGraphPass
	{
	public:
		SSAOMainPass(Ref<Texture> normalsTexture, Ref<Texture> depthTexture);

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
	private:
		Ref<Material> m_Material = nullptr;

		Ref<Texture> m_NormalsTexture = nullptr;
		Ref<Texture> m_DepthTexture = nullptr;

		Ref<SSAO> m_Parameters = nullptr;
	};

	class Grapple_API SSAOComposingPass : public RenderGraphPass
	{
	public:
		SSAOComposingPass(Ref<Texture> colorTexture, Ref<Texture> aoTexture);

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
	private:
		Ref<Texture> m_ColorTexture = nullptr;

		Ref<Texture> m_AOTexture = nullptr;
		Ref<Material> m_Material = nullptr;

		Ref<SSAO> m_Parameters = nullptr;
	};
}
