#pragma once

#include "GrappleECS/Entity/Archetype.h"
#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/QueryFilters.h"

#include <vector>
#include <unordered_set>

namespace Grapple
{
	using QueryId = size_t;
	constexpr QueryId INVALID_QUERY_ID = SIZE_MAX;

	struct QueryData
	{
		QueryId Id;

		std::vector<ComponentId> Components;
		std::unordered_set<ArchetypeId> MatchedArchetypes;
	};
}