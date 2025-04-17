#include "Commands.h"

#include "GrappleECS/World.h"

namespace Grapple
{
	AddComponentCommand::AddComponentCommand(FutureEntity entity, ComponentId component, ComponentInitializationStrategy initStrategy)
		: m_Entity(entity), m_Component(component), m_InitStrategy(initStrategy) {}
	
	void AddComponentCommand::Apply(CommandContext& context, World& world)
	{
		Grapple_CORE_ASSERT(world.IsEntityAlive(context.GetEntity(m_Entity)));
		world.Entities.AddEntityComponent(context.GetEntity(m_Entity), m_Component, nullptr, m_InitStrategy);
	}

	RemoveComponentCommand::RemoveComponentCommand(FutureEntity entity, ComponentId component)
		: m_Entity(entity), m_Component(component) {}

	void RemoveComponentCommand::Apply(CommandContext& context, World& world)
	{
		Grapple_CORE_ASSERT(world.IsEntityAlive(context.GetEntity(m_Entity)));
		world.Entities.RemoveEntityComponent(context.GetEntity(m_Entity), m_Component);
	}

	void DeleteEntityCommand::Apply(CommandContext& context, World& world)
	{
		world.DeleteEntity(m_Entity);
	}

	GetEntityCommand::GetEntityCommand(Entity entity)
		: m_Entity(entity) {}

	void GetEntityCommand::Apply(CommandContext& context, World& world)
	{
		Grapple_CORE_ASSERT(world.IsEntityAlive(m_Entity));
		context.SetEntity(m_OutputEntity, m_Entity);
	}

	void GetEntityCommand::Initialize(FutureEntity entity)
	{
		m_OutputEntity = entity;
	}
}
