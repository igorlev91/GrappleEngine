#include <GrappleECS/World.h>
#include <GrappleECS/System/System.h>

#include <GrappleECS/System/SystemInitializer.h>
#include <GrappleECS/Entity/ComponentInitializer.h>

#include <GrappleECS/Commands/CommandBuffer.h>

#include <Grapple/Core/Time.h>
#include <GrappleCore/Serialization/TypeInitializer.h>

#include <Grapple/AssetManager/AssetManager.h>

#include <Grapple/Scene/Components.h>
#include <Grapple/Scene/Prefab.h>

#include <Grapple/Input/InputManager.h>

#include <iostream>
#include <random>

namespace Sandbox
{
	using namespace Grapple;
	struct RotatingQuadData
	{
		Grapple_COMPONENT;
		float RotationSpeed;
		AssetHandle PrefabHandle;
	};
	Grapple_IMPL_COMPONENT(RotatingQuadData,
		Grapple_PROPERTY(RotatingQuadData, RotationSpeed),
		Grapple_PROPERTY(RotatingQuadData, PrefabHandle)
	);

	struct SomeComponent
	{
		Grapple_COMPONENT;

		int a, b;

		SomeComponent()
			: a(100), b(-234) {}
	};
	Grapple_IMPL_COMPONENT(SomeComponent,
		Grapple_PROPERTY(SomeComponent, a),
		Grapple_PROPERTY(SomeComponent, b),
	);

	struct TestComponent
	{
		Grapple_COMPONENT;
		int a;

		TestComponent()
			: a(1000) {}
	};
	Grapple_IMPL_COMPONENT(TestComponent, Grapple_PROPERTY(TestComponent, a));

	class RotatingQuadSystem : public Grapple::System
	{
	public:
		Grapple_SYSTEM;

		RotatingQuadSystem() {}

		virtual void OnConfig(SystemConfig& config) override
		{
			m_Query = World::GetCurrent().NewQuery().With<TransformComponent, SpriteComponent>().Create();
			m_SingletonQuery = World::GetCurrent().NewQuery().With<RotatingQuadData>().Create();

			m_DeletionQuery = World::GetCurrent().NewQuery()
				.Deleted()
				.With<SomeComponent>()
				.Create();
		}

		virtual void OnUpdate(SystemExecutionContext& context) override
		{
			std::optional<const RotatingQuadData*> data = World::GetCurrent().TryGetSingletonComponent<const RotatingQuadData>();
			if (data.has_value())
			{
				Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(data.value()->PrefabHandle);

				static std::random_device s_Device;
				static std::mt19937_64 s_Engine(s_Device());
				static std::uniform_real_distribution<float> s_UniformDistricution(-50.0f, 50.0f);

				uint32_t op = 0;
				if (InputManager::IsKeyPressed(KeyCode::D1))
					op = 1;
				if (InputManager::IsKeyPressed(KeyCode::D2))
					op = 2;
				if (InputManager::IsKeyPressed(KeyCode::D3))
					op = 3;
				if (InputManager::IsKeyPressed(KeyCode::D4))
					op = 4;
				if (InputManager::IsKeyPressed(KeyCode::D5))
					op = 5;
				if (InputManager::IsKeyPressed(KeyCode::D6))
					op = 6;
				if (InputManager::IsKeyPressed(KeyCode::D7))
					op = 7;

				for (EntityView chunk : m_Query)
				{
					auto transforms = chunk.View<TransformComponent>();
					uint32_t entityIndex = 0;
					for (EntityViewElement entity : chunk)
					{
						Entity e = chunk.GetEntity(entityIndex).value_or(Entity());
						switch (op)
						{
						case 1:
						{
							glm::vec3 position = glm::vec3(s_UniformDistricution(s_Engine), s_UniformDistricution(s_Engine), 0.0f);
							context.Commands->AddEntityCommand(InstantiatePrefab(prefab))
								.AddComponent<SomeComponent>()
								.SetComponent<TransformComponent>(TransformComponent{ position });
							break;
						}
						case 2:
							context.Commands->GetEntity(e)
								.AddComponent<SomeComponent>();
							break;
						case 3:
							context.Commands->CreateEntity<TransformComponent, SpriteComponent, SomeComponent>();
							break;
						case 4:
							context.Commands->DeleteEntity(e);
							break;
						case 5:
							context.Commands->CreateEntity(TransformComponent{ glm::vec3(1.0f, 0.0f, 0.0f) }, SomeComponent{});
							break;
						case 6:
							context.Commands->GetEntity(e)
								.RemoveComponent<SomeComponent>();
							break;
						}

						transforms[entity].Rotation.z += data.value()->RotationSpeed * Time::GetDeltaTime();

						entityIndex++;
					}
				}
			}

			for (EntityView view : m_DeletionQuery)
			{
				for (EntityViewIterator entity = view.begin(); entity != view.end(); ++entity)
				{
					auto id = view.GetEntity(entity.GetEntityIndex());
					if (id.has_value())
						Grapple_INFO("{}", id->GetIndex());
				}
			}
		}
	private:
		Query m_Query;
		Query m_SingletonQuery;
		Query m_DeletionQuery;
	};
	//Grapple_IMPL_SYSTEM(RotatingQuadSystem);
}
