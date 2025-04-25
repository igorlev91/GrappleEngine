#include "QueyrBuilder.h"

namespace Grapple
{
	QueryBuilder::QueryBuilder(QueryCache& queries)
		: m_Queries(queries) {}

	Query QueryBuilder::Create()
	{
		return m_Queries.AddQuery(ComponentSet(m_Ids));
	}
}
