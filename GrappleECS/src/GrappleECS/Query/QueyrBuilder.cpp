#include "QueyrBuilder.h"

namespace Grapple
{
	QueryBuilder::QueryBuilder(QueryCache& queries)
		: m_Queries(queries)
	{
		m_Data.Target = QueryTarget::AllEntities;
	}

	Query QueryBuilder::Create()
	{
		return m_Queries.CreateQuery(m_Data);
	}
}
