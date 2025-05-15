#pragma once

#include "GrappleECS/World.h"
#include "GrappleECS/System/System.h"
#include "GrappleECS/System/SystemInitializer.h"

#include "Grapple/Renderer/Material.h"

namespace Grapple
{
	class LightVisualizer : public System
	{
	public:
		Grapple_SYSTEM;

		void OnConfig(World& world, SystemConfig& config) override;
		void OnUpdate(World& world, SystemExecutionContext& context) override;
	private:
		void ReloadShaders();
	private:
		Query m_DirectionalLightQuery;
		Query m_PointLightsQuery;
		Query m_SpotlightsQuery;

		Ref<Material> m_DebugIconsMaterial;
		bool m_HasProjectOpenHandler = false;
	};
}