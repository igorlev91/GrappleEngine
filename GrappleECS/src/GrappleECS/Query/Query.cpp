#include "Query.h"

namespace Grapple
{
	QueryId Query::GetId() const
	{
		return m_Id;
	}

	QueryIterator Query::begin() const
	{
		Grapple_CORE_ASSERT(m_Entities);
		Grapple_CORE_ASSERT(m_QueryCache);
		return QueryIterator(*m_Entities, (*m_QueryCache)[m_Id].MatchedArchetypes.begin());
	}

	QueryIterator Query::end() const
	{
		Grapple_CORE_ASSERT(m_Entities);
		Grapple_CORE_ASSERT(m_QueryCache);
		return QueryIterator(*m_Entities, (*m_QueryCache)[m_Id].MatchedArchetypes.end());
	}

	const std::unordered_set<ArchetypeId>& Query::GetMatchedArchetypes() const
	{
		return (*m_QueryCache)[m_Id].MatchedArchetypes;
	}

	size_t Query::GetEntitiesCount() const
	{
		size_t count = 0;
		for (ArchetypeId archetype : (*m_QueryCache)[m_Id].MatchedArchetypes)
			count += m_Entities->GetEntityStorage(archetype).GetEntitiesCount();

		return count;
	}
}