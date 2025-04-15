#pragma once

#include "GrappleECS/Entity/ComponentGroup.h"
#include "GrappleECS/Commands/Command.h"

namespace Grapple
{
	class GrappleECS_API AddComponentCommand : public Command
	{
	public:
		AddComponentCommand(Entity entity,
			ComponentId component,
			ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor);
	public:
		virtual void Apply(World& world) override;
	private:
		ComponentId m_Component;
		Entity m_Entity;
		ComponentInitializationStrategy m_InitStrategy;
	};

	class GrappleECS_API RemoveComponentCommand : public Command
	{
	public:
		RemoveComponentCommand(Entity entity, ComponentId component);

		virtual void Apply(World& world) override;
	private:
		Entity m_Entity;
		ComponentId m_Component;
	};
}