#pragma once

#include "Grapple/Core/Assert.h"

#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Entity/Archetype.h"

#include "GrappleECS/Query/QueryData.h"

#include <vector>
#include <unordered_map>

namespace Grapple
{
	class GrappleECS_API Registry;

	class Query;
	class GrappleECS_API QueryCache
	{
	public:
		QueryCache(Registry& registry)
			: m_Registry(registry) {}

		const QueryData& operator[](QueryId id) const;
	public:
		Query AddQuery(const ComponentSet& components);
		void OnArchetypeCreated(ArchetypeId archetype);
	private:
		bool CompareComponentSets(const std::vector<ComponentId>& archetypeComponents, const std::vector<ComponentId>& queryComponents);
	private:
		Registry& m_Registry;

		std::vector<QueryData> m_Queries;
		std::unordered_map<ComponentId, std::vector<QueryId>> m_CachedMatches;
	};
}