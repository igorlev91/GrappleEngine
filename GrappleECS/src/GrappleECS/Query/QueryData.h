#pragma once

#include "GrappleECS/Entity/Component.h"

#include <vector>
#include <unordered_set>

namespace Grapple
{
	struct QueryData
	{
		std::vector<ComponentId> Components;
		std::unordered_set<ArchetypeId> MatchedArchetypes;
	};
}