#pragma once

#include "GrappleCore/Core.h"

#include "GrappleECS/Query/QueryCache.h"
#include "GrappleECS/Query/QueryFilters.h"
#include "GrappleECS/Query/Query.h"

namespace Grapple
{
	class GrappleECS_API QueryBuilder
	{
	public:
		QueryBuilder(QueryCache& queries);

		template<typename... T>
		QueryBuilder& With()
		{
			size_t count = sizeof...(T);
			m_Data.Components.reserve(m_Data.Components.size() + count);

			size_t index = 0;

			([&]
			{
				m_Data.Components.push_back(COMPONENT_ID(T));
			} (), ...);

			return *this;
		}

		template<typename T>
		QueryBuilder& Without()
		{
			size_t count = sizeof...(T);
			m_Data.Components.reserve(m_Data.Components.size() + count);

			size_t index = 0;

			([&]
			{
				ComponentId id = COMPONENT_ID(T);
				m_Data.Components.push_back(ComponentId(
					id.GetIndex() | (uint32_t)QueryFilterType::Without, 
					id.GetGeneration()));
			} (), ...);

			return *this;
		}

		Query Create();
	private:
		QueryCache& m_Queries;
		QueryCreationData m_Data;
	};
}