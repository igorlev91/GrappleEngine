#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore//Serialization/SerializationStream.h"

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Material.h"

namespace Grapple
{
	class Grapple_API ToneMapping : public RenderPass
	{
	public:
		Grapple_TYPE;

		ToneMapping();

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled;
	private:
		std::optional<uint32_t> m_ColorTexture;
		Ref<Material> m_Material;
	};

	template<>
	struct TypeSerializer<ToneMapping>
	{
		void OnSerialize(ToneMapping& toneMapping, SerializationStream& stream)
		{
			stream.Serialize("Enabled", SerializationValue(toneMapping.Enabled));
		}
	};
}