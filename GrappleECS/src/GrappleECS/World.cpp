#include "World.h"

namespace Grapple
{
	World* s_CurrentWorld = nullptr;

	World::World()
		: m_SystemsManager(*this),
		Entities(Components, m_Queries, m_Archetypes),
		m_Queries(Entities, m_Archetypes)
	{
		Grapple_CORE_ASSERT(s_CurrentWorld == nullptr, "Multiple ECS Worlds");
		s_CurrentWorld = this;
	}

	World::~World()
	{
		s_CurrentWorld = nullptr;
	}

	void World::DeleteEntity(Entity entity)
	{
		Entities.DeleteEntity(entity);
	}

	bool World::IsEntityAlive(Entity entity) const
	{
		return Entities.IsEntityAlive(entity);
	}

	const std::vector<ComponentId>& World::GetEntityComponents(Entity entity)
	{
		return Entities.GetEntityComponents(entity);
	}

	Entity World::GetSingletonEntity(const Query& query)
	{
		return Entities.GetSingletonEntity(query).value_or(Entity());
	}

	World& World::GetCurrent()
	{
		Grapple_CORE_ASSERT(s_CurrentWorld != nullptr);
		return *s_CurrentWorld;
	}
}