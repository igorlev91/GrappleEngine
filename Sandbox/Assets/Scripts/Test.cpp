#include "GrappleScriptingCore/Grapple.h"

#include "Grapple/Core/Log.h"

#include <stdint.h>
#include <iostream>

#include <glm/glm.hpp>

namespace Sandbox
{
	struct Transform
	{
		Grapple_COMPONENT(Transform);

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;
	};
	Grapple_COMPONENT_ALIAS_IMPL(Transform, "struct Grapple::TransformComponent");

	struct HealthComponent
	{
		Grapple_COMPONENT(HealthComponent);
	};
	Grapple_COMPONENT_IMPL(HealthComponent);

	struct TestSystem : public Grapple::SystemBase
	{
		Grapple_SYSTEM(TestSystem);

		virtual void Configure(Grapple::SystemConfiguration& config) override
		{
		}

		virtual void Execute(Grapple::EntityView& chunk) override
		{
			Grapple::ComponentView<Transform> transforms = chunk.View<Transform>();

			for (Grapple::EntityElement entity : chunk)
			{
				Transform& transform = transforms[entity];
				Grapple_INFO("Position: {0}, {1}, {2}", transform.Position.x, transform.Position.y, transform.Position.z);
			}
		}
	};
	Grapple_SYSTEM_IMPL(TestSystem);
}