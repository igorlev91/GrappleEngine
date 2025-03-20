#pragma once

#include "GrappleECS/Component.h"
#include "GrappleECS/EntityStorage/EntityStorage.h"

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

		std::unordered_map<ComponentId, ArchetypeEdge> Edges;

		Archetype()
			: Id(INVALID_ARCHETYPE_ID) {}

		Archetype(Archetype&& other) noexcept
			: Id(other.Id), Components(std::move(other.Components)), ComponentOffsets(std::move(other.ComponentOffsets)), Edges(std::move(other.Edges))
		{
			other.Id = INVALID_ARCHETYPE_ID;
		}
	};
	
	struct ArchetypeRecord
	{
		Archetype Data;
		EntityStorage Storage;

		ArchetypeRecord() {}

		ArchetypeRecord(ArchetypeRecord&& other) noexcept
			: Data(std::move(other.Data)), Storage(std::move(other.Storage)) {}
	};
}