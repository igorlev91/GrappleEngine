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

		AtmospherePass()
			: RenderPass(RenderPassQueue::PostProcessing) {}

		void OnRender(RenderingContext& context) override;
	public:
		bool Enabled = true;
		Ref<Material> AtmosphereMaterial = nullptr;

		// All units are Km
		float PlanetRadius = 6360.0f;
		float AtmosphereThickness = 100.0f;
		float MieHeight = 1.2f;
		float RayleighHeight = 8.0f;

		float ObserverHeight = 0.200f;

		uint32_t ViewRaySteps = 10;
		uint32_t SunTransmittanceSteps = 10;
	};

	template<>
	struct TypeSerializer<AtmospherePass>
	{
		void OnSerialize(AtmospherePass& atmosphere, SerializationStream& stream)
		{
			stream.Serialize("Enabled", SerializationValue(atmosphere.Enabled));
			stream.Serialize("AtmosphereMaterial", SerializationValue(atmosphere.AtmosphereMaterial));
			stream.Serialize("PlaneRadius", SerializationValue(atmosphere.PlanetRadius));
			stream.Serialize("AtmosphereThickness", SerializationValue(atmosphere.AtmosphereThickness));
			stream.Serialize("MieHeight", SerializationValue(atmosphere.MieHeight));
			stream.Serialize("RayleighHeight", SerializationValue(atmosphere.RayleighHeight));
			stream.Serialize("ObserverHeight", SerializationValue(atmosphere.ObserverHeight));
			stream.Serialize("ViewRaySteps", SerializationValue(atmosphere.ViewRaySteps));
			stream.Serialize("SunTransmittanceSteps", SerializationValue(atmosphere.SunTransmittanceSteps));
		}
	};
}
