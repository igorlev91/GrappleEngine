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

		float Speed;
		
		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
			Grapple_SERIALIZE_FIELD(settings, MovingQuadComponet, Speed);
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
		}

		virtual void Execute(Grapple::EntityView& chunk) override
		{
			glm::vec3 direction = glm::vec3(0.0f);

			if (Grapple::Input::IsKeyPressed(Grapple::KeyCode::W))
				direction.y += 1;
			if (Grapple::Input::IsKeyPressed(Grapple::KeyCode::S))
				direction.y -= 1;
			if (Grapple::Input::IsKeyPressed(Grapple::KeyCode::A))
				direction.x -= 1;
			if (Grapple::Input::IsKeyPressed(Grapple::KeyCode::D))
				direction.x += 1;

			Grapple::ComponentView<Grapple::Transform> transforms = chunk.View<Grapple::Transform>();
			Grapple::ComponentView<Grapple::Sprite> sprites = chunk.View<Grapple::Sprite>();
			Grapple::ComponentView<MovingQuadComponet> quadComponents = chunk.View<MovingQuadComponet>();

			for (Grapple::EntityElement entity : chunk)
			{
				Grapple::Transform& transform = transforms[entity];
				Grapple::Sprite& sprite = sprites[entity];

				transform.Position += direction * quadComponents[entity].Speed * Grapple::Time::GetDeltaTime();
			}
		}
	};
	Grapple_SYSTEM_IMPL(TestSystem);
}