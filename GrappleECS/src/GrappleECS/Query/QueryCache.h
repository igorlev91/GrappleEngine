#pragma once

#include "Grapple/Core/Assert.h"

#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Entity/Archetype.h"

#include "GrappleECS/Query/QueryData.h"

#include <vector>
#include <unordered_map>

namespace Grapple
{
	class Registry;

	using QueryId = size_t;
	constexpr QueryId INVALID_QUERY_ID = SIZE_MAX;

	class QueryCache
	{
	public:
		QueryCache(Registry& registry)
			: m_Registry(registry) {}
	public:
		QueryId AddQuery(const ComponentSet& components);
		void OnArchetypeCreated(ArchetypeId archetype);

		inline const QueryData& operator[](QueryId id) const
		{
			Grapple_CORE_ASSERT(id < m_Queries.size());
			return m_Queries[id];
		}
	private:
		bool CompareComponentSets(const ComponentSet& archetypeComponents, const ComponentSet& queryComponents);
	private:
		Registry& m_Registry;

		std::vector<QueryData> m_Queries;
		std::unordered_map<ComponentId, std::vector<QueryId>> m_CachedMatches;
	};
}