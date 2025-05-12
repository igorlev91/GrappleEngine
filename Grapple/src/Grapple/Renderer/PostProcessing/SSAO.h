#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore//Serialization/SerializationStream.h"

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Material.h"

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
		Ref<FrameBuffer> m_AOTargets[2];
		Ref<Texture> m_RandomVectors;
		Ref<Material> m_Material;
		Ref<Material> m_BlurMaterial;

		std::optional<uint32_t> m_BiasPropertyIndex;
		std::optional<uint32_t> m_RadiusPropertyIndex;
		std::optional<uint32_t> m_NoiseScalePropertyIndex;

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
}
