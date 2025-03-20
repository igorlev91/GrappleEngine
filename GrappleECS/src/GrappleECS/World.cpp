#include "World.h"

#include "GrappleECS/Query/EntityArchetypesView.h"

namespace Grapple
{
	void World::RegisterSystem(QueryId query, const SystemFunction& system)
	{
		System& systemData = m_Systems.emplace_back();
		systemData.Archetype = INVALID_ARCHETYPE_ID;
		systemData.IsArchetypeQuery = false;
		systemData.Function = system;
		systemData.Query = query;
	}

	void World::OnUpdate()
	{
		for (const System& system : m_Systems)
		{
			if (system.IsArchetypeQuery)
			{
				Grapple_CORE_ASSERT(false, "Not implemented");
			}
			else
			{
				EntityArchetypesView queryResult = m_Registry.ExecuteQuery(system.Query);
				for (EntityView view : queryResult)
					system.Function(view);
			}
		}
	}
}