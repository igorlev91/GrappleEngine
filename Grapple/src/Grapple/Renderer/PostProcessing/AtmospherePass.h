#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore//Serialization/SerializationStream.h"

#include "Grapple/Renderer/RenderPass.h"
#include "Grapple/Renderer/Material.h"

namespace Grapple
{
	class Grapple_API AtmospherePass : public RenderPass
	{
	public:
		Grapple_TYPE;

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled = true;
		Ref<Material> AtmosphereMaterial = nullptr;
	};

	template<>
	struct TypeSerializer<AtmospherePass>
	{
		void OnSerialize(AtmospherePass& atmosphere, SerializationStream& stream)
		{
			stream.Serialize("Enabled", SerializationValue(atmosphere.Enabled));
			stream.Serialize("AtmosphereMaterial", SerializationValue(atmosphere.AtmosphereMaterial));
		}
	};
}
