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

	struct TestSingleton
	{
		Grapple_COMPONENT(TestSingleton);

		float Num;

		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
			Grapple_SERIALIZE_FIELD(settings, TestSingleton, Num);
		}
	};
	Grapple_COMPONENT_IMPL(TestSingleton, TestSingleton::ConfigureSerialization);

	struct TestSystem : public Grapple::SystemBase
	{
		Grapple_SYSTEM(TestSystem);

		virtual void Configure(Grapple::SystemConfiguration& config) override
		{
			m_TestQuery = Grapple::Query::New<Grapple::With<Grapple::Transform>, Grapple::With<MovingQuadComponet>>();
			m_SingletonQuery = Grapple::Query::New<Grapple::With<TestSingleton>>();
		}

		static void ConfigureSerialization(Grapple::TypeSerializationSettings& settings)
		{
		}

		virtual void OnUpdate() override
		{
			TestSingleton* singleton = Grapple::World::GetSingletonComponent<TestSingleton>();
			if (singleton != nullptr)
				Grapple_INFO(singleton->Num);

			Grapple::World::ForEachChunk(m_TestQuery, [](Grapple::EntityView& view)
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

				Grapple::ComponentView<Grapple::Transform> transforms = view.View<Grapple::Transform>();
				Grapple::ComponentView<Grapple::Sprite> sprites = view.View<Grapple::Sprite>();
				Grapple::ComponentView<MovingQuadComponet> quadComponents = view.View<MovingQuadComponet>();
					
				for (Grapple::EntityElement entity : view)
				{
					Grapple::Transform& transform = transforms[entity];
					Grapple::Sprite& sprite = sprites[entity];

					transform.Position += direction * quadComponents[entity].Speed * Grapple::Time::GetDeltaTime();
				}
			});
		}
	private:
		Grapple::Query m_TestQuery;
		Grapple::Query m_SingletonQuery;
	};
	Grapple_SYSTEM_IMPL(TestSystem);
}