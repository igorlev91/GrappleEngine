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

		glm::vec2 Vector2;
		glm::vec3 Vector3;

		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
			Grapple_SERIALIZE_FIELD(settings, HealthComponent, Vector2);
			Grapple_SERIALIZE_FIELD(settings, HealthComponent, Vector3);
		}
	};
	Grapple_COMPONENT_IMPL(HealthComponent, HealthComponent::ConfigureSerialization);

	struct TestSystem : public Grapple::SystemBase
	{
		Grapple_SYSTEM(TestSystem);

		virtual void Configure(Grapple::SystemConfiguration& config) override
		{
			config.Query.Add<Transform>();
			config.Query.Add<HealthComponent>();
		}

		virtual void Execute(Grapple::EntityView& chunk) override
		{
			Grapple::ComponentView<Transform> transforms = chunk.View<Transform>();
			Grapple::ComponentView<HealthComponent> healthComponents = chunk.View<HealthComponent>();

			for (Grapple::EntityElement entity : chunk)
			{
				Transform& transform = transforms[entity];
				transform.Position += healthComponents[entity].Vector3 * Grapple::Time::GetDeltaTime();
			}
		}
	};
	Grapple_SYSTEM_IMPL(TestSystem);
}