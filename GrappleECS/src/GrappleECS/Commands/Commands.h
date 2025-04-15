#pragma once

#include "GrappleECS/Registry.h"
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

	template<typename... T>
	class CreateEntityCommand : public Command
	{
	public:
		CreateEntityCommand(ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor)
			: m_InitStrategy(initStrategy) {}

		virtual void Apply(World& world) override
		{
			world.CreateEntity<T...>(m_InitStrategy);
		}
	private:
		ComponentInitializationStrategy m_InitStrategy;
	};

	class GrappleECS_API DeleteEntityCommand : public Command
	{
	public:
		DeleteEntityCommand(Entity entity)
			: m_Entity(entity) {}

		virtual void Apply(World& world) override;
	private:
		Entity m_Entity;
	};
}