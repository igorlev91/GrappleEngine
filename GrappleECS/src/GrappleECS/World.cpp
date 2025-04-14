#include "World.h"

namespace Grapple
{
	World* s_CurrentWorld = nullptr;

	World::World()
	{
		Grapple_CORE_ASSERT(s_CurrentWorld == nullptr, "Multiple ECS Worlds");
		s_CurrentWorld = this;
	}

	World::~World()
	{
		s_CurrentWorld = nullptr;
	}

	World& World::GetCurrent()
	{
		Grapple_CORE_ASSERT(s_CurrentWorld != nullptr);
		return *s_CurrentWorld;
	}

	Registry& World::GetRegistry()
	{
		return m_Registry;
	}

	SystemsManager& World::GetSystemsManager()
	{
		return m_SystemsManager;
	}

	const SystemsManager& World::GetSystemsManager() const
	{
		return m_SystemsManager;
	}
}