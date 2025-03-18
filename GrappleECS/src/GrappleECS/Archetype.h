#pragma once

#include "GrappleECS/Component.h"
#include "GrappleECS/EntityStorage.h"

#include <vector>
#include <optional>
#include <unordered_map>

namespace Grapple
{
	using ArchetypeId = size_t;
	constexpr ArchetypeId INVALID_ARCHETYPE_ID = SIZE_MAX;

	struct ArchetypeEdge
	{
		ArchetypeId Add;
		ArchetypeId Remove;
	};

	struct Archetype
	{
		size_t Id;
		std::vector<ComponentId> Components; // Sorted
		std::vector<size_t> ComponentOffsets;

		std::optional<size_t> FindComponent(ComponentId component);

		std::unordered_map<ComponentId, ArchetypeEdge> Edges;
	};
	
	struct ArchetypeRecord
	{
		Archetype Data;
		EntityStorage Storage;
	};
}