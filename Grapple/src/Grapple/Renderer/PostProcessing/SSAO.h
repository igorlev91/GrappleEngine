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
		Grapple_SERIALIZABLE;

		SSAO();

		void OnRender(RenderingContext& context) override;
	public:
		float Bias;
		float Radius;
	private:
		Ref<FrameBuffer> m_AOTargets[2];
		Ref<Material> m_Material;

		std::optional<uint32_t> m_BiasPropertyIndex;
		std::optional<uint32_t> m_RadiusPropertyIndex;
	};

	template<>
	struct TypeSerializer<SSAO>
	{
		void OnSerialize(SSAO& ssao, SerializationStream& stream)
		{
			stream.Serialize("Radius", SerializationValue(ssao.Radius));
			stream.Serialize("Bias", SerializationValue(ssao.Bias));
		}
	};
}
