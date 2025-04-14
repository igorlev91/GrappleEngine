#include <GrappleECS/World.h>
#include <GrappleECS/System/System.h>

#include <GrappleECS/System/SystemInitializer.h>
#include <GrappleECS/Entity/ComponentInitializer.h>

#include <Grapple/Serialization/TypeInitializer.h>
#include <Grapple/Scene/Components.h>

#include <iostream>


namespace Sandbox
{
	using namespace Grapple;
	class SandboxTestSystem : public Grapple::System
	{
	public:
		Grapple_SYSTEM;
		Grapple_TYPE;

		virtual void OnConfig(SystemConfig& config) override
		{
			m_TestQuery = World::GetCurrent().CreateQuery<With<TransformComponent>, Without<CameraComponent>>();
		}

		virtual void OnUpdate(SystemExecutionContext& context) override
		{
			for (EntityView chunk : m_TestQuery)
			{
				auto transforms = chunk.View<TransformComponent>();
				for (EntityViewElement entity : chunk)
				{
					transforms[entity].Rotation.z += 2.0f;
				}
			}
		}

		int a;
		float b;
	private:
		Query m_TestQuery;
	};
	Grapple_IMPL_SYSTEM(SandboxTestSystem);
	Grapple_IMPL_TYPE(SandboxTestSystem,
		Grapple_FIELD(SandboxTestSystem, a),
		Grapple_FIELD(SandboxTestSystem, b)
	);

	struct SomeComponent
	{
		Grapple_COMPONENT2;

		int a, b;
	};
	Grapple_IMPL_COMPONENT2(SomeComponent,
		Grapple_FIELD(SomeComponent, a),
		Grapple_FIELD(SomeComponent, b),
	);
}
