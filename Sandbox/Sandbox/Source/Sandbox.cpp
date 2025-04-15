#include <GrappleECS/World.h>
#include <GrappleECS/System/System.h>

#include <GrappleECS/System/SystemInitializer.h>
#include <GrappleECS/Entity/ComponentInitializer.h>

#include <Grapple/Core/Time.h>
#include <Grapple/Serialization/TypeInitializer.h>
#include <Grapple/Scene/Components.h>

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

	class RotatingQuadSystem : public Grapple::System
	{
	public:
		Grapple_SYSTEM;

		virtual void OnConfig(SystemConfig& config) override
		{
			m_Query = World::GetCurrent().CreateQuery<With<TransformComponent>, Without<CameraComponent>>();
			m_SingletonQuery = World::GetCurrent().CreateQuery<With<RotatingQuadData>>();
		}

		virtual void OnUpdate(SystemExecutionContext& context) override
		{
			const RotatingQuadData& data = World::GetCurrent().GetSingletonComponent<RotatingQuadData>();
			Entity e = World::GetCurrent().GetSingletonEntity(m_SingletonQuery);

			for (EntityView chunk : m_Query)
			{
				auto transforms = chunk.View<TransformComponent>();
				for (EntityViewElement entity : chunk)
				{
					transforms[entity].Rotation.z += data.RotationSpeed * Time::GetDeltaTime();
				}
			}
		}
	private:
		Query m_Query;
		Query m_SingletonQuery;
	};
	Grapple_IMPL_SYSTEM(RotatingQuadSystem);
		
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
}
