#include "QueyrBuilder.h"

#include "GrappleCore/Profiler/Profiler.h"

namespace Grapple
{
	template<>
	GrappleECS_API Query QueryBuilder<Query>::Build()
	{
		Grapple_PROFILE_FUNCTION();
		return Query(m_Queries.CreateQuery(m_Data), m_Entities, m_Queries);
	}

	template<>
	GrappleECS_API CreatedEntitiesQuery QueryBuilder<CreatedEntitiesQuery>::Build()
	{
		Grapple_PROFILE_FUNCTION();
		return CreatedEntitiesQuery(m_Queries.CreateQuery(m_Data), m_Entities, m_Queries);
	}
}
