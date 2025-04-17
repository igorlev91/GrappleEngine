#pragma once

#include "GrappleECS/Entities.h"
#include "GrappleECS/Entity/ComponentGroup.h"
#include "GrappleECS/Commands/Command.h"

namespace Grapple
{
	class GrappleECS_API AddComponentCommand : public Command
	{
	public:
		AddComponentCommand()
			: m_InitStrategy(ComponentInitializationStrategy::Zero) {}

		AddComponentCommand(FutureEntity entity,
			ComponentId component,
			ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor);
	public:
		virtual void Apply(CommandContext& context, World& world) override;
	private:
		ComponentId m_Component;
		FutureEntity m_Entity;
		ComponentInitializationStrategy m_InitStrategy;
	};

	template<typename T>
	class AddComponentWithDataCommand : public Command
	{
	public:
		AddComponentWithDataCommand() = default;
		AddComponentWithDataCommand(FutureEntity entity, const T& data)
			: m_Entity(entity), m_Data(data) {}
	public:
		virtual void Apply(CommandContext& context, World& world) override
		{
			Grapple_CORE_ASSERT(world.IsEntityAlive(context.GetEntity(m_Entity)));
			world.AddEntityComponent<T>(context.GetEntity(m_Entity), m_Data);
		}
	private:
		FutureEntity m_Entity;
		T m_Data;
	};

	class GrappleECS_API RemoveComponentCommand : public Command
	{
	public:
		RemoveComponentCommand() = default;
		RemoveComponentCommand(FutureEntity entity, ComponentId component);

		virtual void Apply(CommandContext& context, World& world) override;
	private:
		FutureEntity m_Entity;
		ComponentId m_Component;
	};

	template<typename... T>
	class CreateEntityCommand : public EntityCommand
	{
	public:
		CreateEntityCommand(ComponentInitializationStrategy initStrategy = ComponentInitializationStrategy::DefaultConstructor)
			: m_InitStrategy(initStrategy) {}

		virtual void Apply(CommandContext& context, World& world) override
		{
			context.SetEntity(m_OutputEntity, world.CreateEntity<T...>(m_InitStrategy));
		}

		virtual void Initialize(FutureEntity entity) override
		{
			m_OutputEntity = entity;
		}
	private:
		FutureEntity m_OutputEntity;
		ComponentInitializationStrategy m_InitStrategy;
	};

	template<typename... T>
	class CreateEntityWithDataCommand : public EntityCommand
	{
	public:
		CreateEntityWithDataCommand() = default;
		CreateEntityWithDataCommand(const T& ...components)
			: m_Components(components...) {}

		virtual void Apply(CommandContext& context, World& world) override
		{
			Entity entity = std::apply([&world](const T& ...components) -> Entity { return world.CreateEntity<T...>(components...); }, m_Components);
			context.SetEntity(m_OutputEntity, entity);
		}

		virtual void Initialize(FutureEntity entity) override
		{
			m_OutputEntity = entity;
		}
	private:
		FutureEntity m_OutputEntity;
		std::tuple<T...> m_Components;
	};

	class GrappleECS_API DeleteEntityCommand : public Command
	{
	public:
		DeleteEntityCommand() = default;
		DeleteEntityCommand(Entity entity)
			: m_Entity(entity) {}

		virtual void Apply(CommandContext& context, World& world) override;
	private:
		Entity m_Entity;
	};
}