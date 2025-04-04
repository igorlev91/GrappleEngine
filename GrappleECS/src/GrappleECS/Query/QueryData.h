#pragma once

#include "GrappleECS/Entity/Archetype.h"
#include "GrappleECS/Entity/Component.h"

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

	enum class ComponentsFiler : uint32_t
	{
		With = 0,
		Without = 1ui32 << 31ui32,
	};
}