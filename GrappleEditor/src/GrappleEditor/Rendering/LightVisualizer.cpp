#include "LightVisualizer.h"

#include "GrappleECS/World.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/Renderer/DebugRenderer.h"

namespace Grapple
{
	Grapple_IMPL_SYSTEM(LightVisualizer);

	void LightVisualizer::OnConfig(SystemConfig& config)
	{
		config.Group = World::GetCurrent().GetSystemsManager().FindGroup("Debug Rendering");
		m_Query = World::GetCurrent().NewQuery().With<TransformComponent, DirectionalLight>().Create();
	}

	void LightVisualizer::OnUpdate(SystemExecutionContext& context)
	{
		for (EntityView view : m_Query)
		{
			auto transforms = view.View<TransformComponent>();
			auto lights = view.View<DirectionalLight>();

			for (EntityViewElement entity : view)
				DebugRenderer::DrawRay(transforms[entity].Position, transforms[entity].TransformDirection(glm::vec3(0.0f, 0.0f, -1.0f)));
		}
	}
}
