#include <GrappleECS/World.h>
#include <GrappleECS/System/System.h>

#include <GrappleECS/System/SystemInitializer.h>
#include <GrappleECS/Entity/ComponentInitializer.h>

#include <GrappleECS/Commands/CommandBuffer.h>

#include <Grapple/Core/Time.h>
#include <Grapple/Serialization/TypeInitializer.h>
#include <Grapple/Scene/Components.h>

#include <Grapple/Input/InputManager.h>

#include <iostream>

namespace Sandbox
{
	using namespace Grapple;
	struct RotatingQuadData
	{
		Grapple_COMPONENT;
		float RotationSpeed;
	};
	Grapple_IMPL_COMPONENT(RotatingQuadData,
		Grapple_FIELD(RotatingQuadData, RotationSpeed)
	);

	struct SomeComponent
	{
		Grapple_COMPONENT;

		int a, b;

		SomeComponent()
			: a(100), b(-234) {}
	};
	Grapple_IMPL_COMPONENT(SomeComponent,
		Grapple_FIELD(SomeComponent, a),
		Grapple_FIELD(SomeComponent, b),
	);

	class RotatingQuadSystem : public Grapple::System
	{
	public:
		Grapple_SYSTEM;

		RotatingQuadSystem() {}

		virtual void OnConfig(SystemConfig& config) override
		{
			m_Query = World::GetCurrent().CreateQuery<With<TransformComponent>, With<SpriteComponent>>();
			m_SingletonQuery = World::GetCurrent().CreateQuery<With<RotatingQuadData>>();
		}

		virtual void OnUpdate(SystemExecutionContext& context) override
		{
			const RotatingQuadData& data = World::GetCurrent().GetSingletonComponent<RotatingQuadData>();

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
						m_CommandBuffer.AddComponent<SomeComponent>(e);
						break;
					case 2:
						m_CommandBuffer.RemoveComponent<SomeComponent>(e);
						break;
					case 3:
						m_CommandBuffer.CreateEntity<TransformComponent, SpriteComponent, SomeComponent>();
						break;
					case 4:
						m_CommandBuffer.DeleteEntity(e);
						break;
					}

					transforms[entity].Rotation.z += data.RotationSpeed * Time::GetDeltaTime();

					entityIndex++;
				}
			}

			m_CommandBuffer.Execute(World::GetCurrent());
		}
	private:
		CommandBuffer m_CommandBuffer;
		Query m_Query;
		Query m_SingletonQuery;
	};
	Grapple_IMPL_SYSTEM(RotatingQuadSystem);
}
