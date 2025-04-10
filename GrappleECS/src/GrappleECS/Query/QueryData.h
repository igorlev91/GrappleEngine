#pragma once

#include "GrappleECS/Entity/Archetype.h"
#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/QueryFilters.h"
#include "GrappleECS/QueryId.h"

#include <vector>
#include <unordered_set>

namespace Grapple
{
	struct QueryData
	{
		QueryId Id;

		std::vector<ComponentId> Components;
		std::unordered_set<ArchetypeId> MatchedArchetypes;
	};
}