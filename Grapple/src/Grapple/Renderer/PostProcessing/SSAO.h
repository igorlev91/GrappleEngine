#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore//Serialization/SerializationStream.h"

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/RenderGraph/RenderGraphPass.h"

namespace Grapple
{
	class Grapple_API SSAO : public RenderPass
	{
	public:
		Grapple_TYPE;

		SSAO();

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled;

		float Bias;
		float Radius;
		float BlurSize;
	private:
		Ref<Material> m_Material;
		Ref<Material> m_BlurMaterial;

		std::optional<uint32_t> m_NormalsTextureIndex;
		std::optional<uint32_t> m_DepthTextureIndex;
		std::optional<uint32_t> m_BiasPropertyIndex;
		std::optional<uint32_t> m_RadiusPropertyIndex;

		std::optional<uint32_t> m_ColorTexture;
		std::optional<uint32_t> m_AOTexture;
		std::optional<uint32_t> m_BlurSizePropertyIndex;
		std::optional<uint32_t> m_TexelSizePropertyIndex;
	};

	template<>
	struct TypeSerializer<SSAO>
	{
		void OnSerialize(SSAO& ssao, SerializationStream& stream)
		{
			stream.Serialize("Enabled", SerializationValue(ssao.Enabled));
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
		float m_Bias = 0.0f;
		float m_Radius = 0.0f;
		Ref<Material> m_Material = nullptr;

		Ref<Texture> m_NormalsTexture = nullptr;
		Ref<Texture> m_DepthTexture = nullptr;
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
	};

	class Grapple_API SSAOBlitPass : public RenderGraphPass
	{
	public:
		SSAOBlitPass(Ref<Texture> colorTexture);

		void OnRender(const RenderGraphContext& context, Ref<CommandBuffer> commandBuffer) override;
	private:
		Ref<Texture> m_ColorTexture = nullptr;
	};
}
