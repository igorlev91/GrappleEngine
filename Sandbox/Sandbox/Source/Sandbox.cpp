#include <Grapple/Scene/Components.h>

#include <GrappleECS/World.h>
#include <GrappleECS/System/System.h>
#include <GrappleECS/System/SystemInitializer.h>

#include <iostream>

namespace Sandbox
{
	using namespace Grapple;
	class TestSandboxSystem : public System
	{
		Grapple_SYSTEM;
		virtual void OnConfig(SystemConfig& config) override
		{
			World& world = World::GetCurrent();
			const std::vector<SystemInitializer*>& inits = SystemInitializer::GetInitializers();
			
			config.Group = World::GetCurrent().GetSystemsManager().FindGroup("Scripting Update");

			m_TestQuery = World::GetCurrent().CreateQuery<With<TransformComponent>>();
		}

		virtual void OnUpdate(Grapple::SystemExecutionContext& context) override
	 	{
			std::cout << "Update\n";
			for (EntityView chunk : m_TestQuery)
			{
				auto transforms = chunk.View<TransformComponent>();
				for (EntityViewElement entity : chunk)
				{
					transforms[entity].Rotation.z += 10.0f;
				}
			}
		}

	private:
		Query m_TestQuery;
	};
	Grapple_IMPL_SYSTEM(TestSandboxSystem);
}
