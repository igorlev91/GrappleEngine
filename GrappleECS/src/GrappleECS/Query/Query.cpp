#include "Query.h"

namespace Grapple
{
	QueryId Query::GetId() const
	{
		return m_Id;
	}

	QueryIterator Query::begin() const
	{
		Grapple_CORE_ASSERT(m_Registry);
		Grapple_CORE_ASSERT(m_QueryCache);
		return QueryIterator(*m_Registry, (*m_QueryCache)[m_Id].MatchedArchetypes.begin());
	}

	QueryIterator Query::end() const
	{
		Grapple_CORE_ASSERT(m_Registry);
		Grapple_CORE_ASSERT(m_QueryCache);
		return QueryIterator(*m_Registry, (*m_QueryCache)[m_Id].MatchedArchetypes.end());
	}

	const std::unordered_set<ArchetypeId>& Query::GetMatchedArchetypes() const
	{
		return (*m_QueryCache)[m_Id].MatchedArchetypes;
	}
}