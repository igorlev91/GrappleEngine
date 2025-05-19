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
		Ref<Material> AtmosphereMaterial = nullptr;
		glm::vec3 SunLightWaveLengths;
	};

	template<>
	struct TypeSerializer<AtmospherePass>
	{
		void OnSerialize(AtmospherePass& atmosphere, SerializationStream& stream)
		{
			stream.Serialize("AtmosphereMaterial", SerializationValue(atmosphere.AtmosphereMaterial));
			stream.Serialize("SunLightWaveLengths", SerializationValue(atmosphere.SunLightWaveLengths));
		}
	};
}
