#include "GrappleScriptingCore/Grapple.h"

#include "Grapple/Core/Log.h"

#include <stdint.h>
#include <iostream>

#include <glm/glm.hpp>

namespace Sandbox
{
	struct MovingQuadComponet
	{
		Grapple_COMPONENT(MovingQuadComponet);

		glm::vec3 Velocity;
		
		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
			Grapple_SERIALIZE_FIELD(settings, MovingQuadComponet, Velocity);
		}
	};
	Grapple_COMPONENT_IMPL(MovingQuadComponet, MovingQuadComponet::ConfigureSerialization);

	struct TestSystem : public Grapple::SystemBase
	{
		Grapple_SYSTEM(TestSystem);

		virtual void Configure(Grapple::SystemConfiguration& config) override
		{
			config.Query.Add<Grapple::Transform>();
			config.Query.Add<MovingQuadComponet>();
			config.Query.Add<Grapple::Sprite>();
		}

		virtual void Execute(Grapple::EntityView& chunk) override
		{
			Grapple::ComponentView<Grapple::Transform> transforms = chunk.View<Grapple::Transform>();
			Grapple::ComponentView<Grapple::Sprite> sprites = chunk.View<Grapple::Sprite>();
			Grapple::ComponentView<MovingQuadComponet> quadComponents = chunk.View<MovingQuadComponet>();

			for (Grapple::EntityElement entity : chunk)
			{
				Grapple::Transform& transform = transforms[entity];
				Grapple::Sprite& sprite = sprites[entity];

				transform.Position += quadComponents[entity].Velocity * Grapple::Time::GetDeltaTime();
				sprite.Color.r = glm::sin(transform.Position.x * 10.0f);
				sprite.Color.g = glm::cos(transform.Position.y * 10.0f);
			}
		}
	};
	Grapple_SYSTEM_IMPL(TestSystem);
}