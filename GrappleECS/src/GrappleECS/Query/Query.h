#pragma once

#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Entity/Archetype.h"

#include "GrappleECS/Query/QueryCache.h"
#include "GrappleECS/Query/EntityView.h"

#include "GrappleECS/Registry.h"

#include <vector>
#include <unordered_set>

namespace Grapple
{
	class QueryIterator
	{
	public:
		QueryIterator(Registry& registry, const std::unordered_set<ArchetypeId>::const_iterator& archetype)
			: m_Registry(registry), m_Archetype(archetype) {}

		inline EntityView operator*()
		{
			return EntityView(m_Registry, *m_Archetype);
		}

		inline QueryIterator operator++()
		{
			m_Archetype++;
			return *this;
		}

		inline bool operator==(const QueryIterator& other)
		{
			return &m_Registry == &other.m_Registry && m_Archetype == other.m_Archetype;
		}

		inline bool operator!=(const QueryIterator& other)
		{
			return &m_Registry != &other.m_Registry || m_Archetype != other.m_Archetype;
		}
	private:
		Registry& m_Registry;
		std::unordered_set<ArchetypeId>::const_iterator m_Archetype;
	};

	class Query
	{
	public:
		constexpr Query()
			: m_Id(INVALID_QUERY_ID), m_Registry(nullptr), m_QueryCache(nullptr) {}
		constexpr Query(QueryId id, Registry& registry, const QueryCache& queryCache)
			: m_Id(id), m_Registry(&registry), m_QueryCache(&queryCache) {}
	public:
		inline QueryId GetId() const { return m_Id; }

		inline QueryIterator begin() const
		{
			Grapple_CORE_ASSERT(m_Registry);
			Grapple_CORE_ASSERT(m_QueryCache);
			return QueryIterator(*m_Registry, (*m_QueryCache)[m_Id].MatchedArchetypes.begin());
		}

		inline QueryIterator end() const
		{
			Grapple_CORE_ASSERT(m_Registry);
			Grapple_CORE_ASSERT(m_QueryCache);
			return QueryIterator(*m_Registry, (*m_QueryCache)[m_Id].MatchedArchetypes.end());
		}
	private:
		QueryId m_Id;
		Registry* m_Registry;
		const QueryCache* m_QueryCache;
	};
}