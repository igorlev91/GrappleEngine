#include "AtmospherePass.h"

#include "Grapple/Renderer/Renderer.h"

namespace Grapple
{
	Grapple_IMPL_TYPE(AtmospherePass);

	void AtmospherePass::OnRender(RenderingContext& context)
	{
		if (!AtmosphereMaterial || !Enabled || !Renderer::GetCurrentViewport().PostProcessingEnabled)
			return;

		Ref<Shader> shader = AtmosphereMaterial->GetShader();
		if (shader == nullptr)
			return;

		std::optional<uint32_t> planetRadius = shader->GetPropertyIndex("u_Params.PlanetRadius");
		std::optional<uint32_t>	atmosphereThickness = shader->GetPropertyIndex("u_Params.AtmosphereThickness");
		std::optional<uint32_t> mieHeight = shader->GetPropertyIndex("u_Params.MieHeight");
		std::optional<uint32_t> rayleighHeight = shader->GetPropertyIndex("u_Params.RayleighHeight");
		std::optional<uint32_t> observerHeight = shader->GetPropertyIndex("u_Params.ObserverHeight");
		std::optional<uint32_t> viewRaySteps = shader->GetPropertyIndex("u_Params.ViewRaySteps");
		std::optional<uint32_t> sunTransmittanceSteps = shader->GetPropertyIndex("u_Params.SunTransmittanceSteps");

		AtmosphereMaterial->WritePropertyValue<float>(*planetRadius, PlanetRadius);
		AtmosphereMaterial->WritePropertyValue<float>(*atmosphereThickness, AtmosphereThickness);
		AtmosphereMaterial->WritePropertyValue<float>(*mieHeight, MieHeight);
		AtmosphereMaterial->WritePropertyValue<float>(*rayleighHeight, RayleighHeight);
		AtmosphereMaterial->WritePropertyValue<float>(*observerHeight, ObserverHeight);

		AtmosphereMaterial->WritePropertyValue<int32_t>(*viewRaySteps, (int32_t)ViewRaySteps);
		AtmosphereMaterial->WritePropertyValue<int32_t>(*sunTransmittanceSteps, (int32_t)SunTransmittanceSteps);

		Renderer::DrawFullscreenQuad(AtmosphereMaterial);
	}
}
