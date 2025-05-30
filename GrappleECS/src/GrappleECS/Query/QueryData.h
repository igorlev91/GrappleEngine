#pragma once

#include "GrappleECS/Entity/Archetype.h"
#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Query/QueryFilters.h"

#include <vector>
#include <unordered_set>

namespace Grapple
{
	using QueryId = size_t;
	constexpr QueryId INVALID_QUERY_ID = SIZE_MAX;

	enum class QueryTarget : uint8_t
	{
		AllEntities,
		DeletedEntities,
		CreatedEntities,
	};

	struct QueryCreationData
	{
		QueryTarget Target;
		std::vector<ComponentId> Components;
	};

	struct QueryData
	{
		QueryId Id;
		QueryTarget Target;

		std::vector<ComponentId> Components;
		std::unordered_set<ArchetypeId> MatchedArchetypes;
	};
}