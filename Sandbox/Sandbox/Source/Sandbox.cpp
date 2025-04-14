#include <GrappleECS/World.h>
#include <GrappleECS/System/System.h>
#include <GrappleECS/System/SystemInitializer.h>

#include <Grapple/Scene/Components.h>

#include <iostream>

namespace Sandbox
{
	using namespace Grapple;
	class SandboxTestSystem : public Grapple::System
	{
	public:
		Grapple_SYSTEM;

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
	private:
		Query m_TestQuery;
	};
	Grapple_IMPL_SYSTEM(SandboxTestSystem);
}
