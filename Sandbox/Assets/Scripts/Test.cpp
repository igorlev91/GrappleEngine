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

	struct TestComponent
	{
		Grapple_COMPONENT(TestComponent);

		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
		}
	};
	Grapple_COMPONENT_IMPL(TestComponent, TestComponent::ConfigureSerialization);

	struct TestSystem : public Grapple::SystemBase
	{
		Grapple_SYSTEM(TestSystem);

		virtual void Configure(Grapple::SystemConfiguration& config) override
		{
			config.Query.With<Grapple::Transform>();
			config.Query.With<MovingQuadComponet>();
			config.Query.Without<TestComponent>();
		}

		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
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

	struct Spawner
	{
		Grapple_COMPONENT(Spawner);
		float Interval;
		float TimeLeft;

		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
			Grapple_SERIALIZE_FIELD(settings, Spawner, Interval);
			Grapple_SERIALIZE_FIELD(settings, Spawner, TimeLeft);
		}
	};
	Grapple_COMPONENT_IMPL(Spawner, Spawner::ConfigureSerialization);

	struct SpawnSystem : public Grapple::SystemBase
	{
		Grapple_SYSTEM(SpawnSystem);

		virtual void Configure(Grapple::SystemConfiguration& config) override
		{
			config.Query.With<Spawner>();
		}

		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
			Grapple_SERIALIZE_FIELD(settings, SpawnSystem, TestValue);
			Grapple_SERIALIZE_FIELD(settings, SpawnSystem, Tex);
			Grapple_SERIALIZE_FIELD(settings, SpawnSystem, Vector);
		}

		virtual void Execute(Grapple::EntityView& chunk) override
		{
			Grapple::ComponentView<Spawner> spawners = chunk.View<Spawner>();

			bool spawn = false;
			for (Grapple::EntityElement entity : chunk)
			{
				Spawner& spawner = spawners[entity];

				if (spawner.TimeLeft >= 0)
					spawner.TimeLeft -= Grapple::Time::GetDeltaTime();
				else
				{
					spawn = true;
					spawner.TimeLeft += spawner.Interval;
				}
			}

			if (spawn)
			{
				Grapple::Entity entity = Grapple::World::CreateEntity<Grapple::Transform, Grapple::Sprite, MovingQuadComponet>();
				Grapple::Transform& transform = Grapple::World::GetEntityComponent<Grapple::Transform>(entity);
				transform.Scale = glm::vec3(1.0f);

				Grapple::Sprite& sprite = Grapple::World::GetEntityComponent<Grapple::Sprite>(entity);
				sprite.Color = glm::vec4(1.0f);

				MovingQuadComponet& movingQuad = Grapple::World::GetEntityComponent<MovingQuadComponet>(entity);
				movingQuad.Speed = 10.0f;
			}
		}
	private:
		float TestValue;
		Grapple::TextureAsset Tex;
		glm::vec3 Vector;
	};
	Grapple_SYSTEM_IMPL(SpawnSystem);
}