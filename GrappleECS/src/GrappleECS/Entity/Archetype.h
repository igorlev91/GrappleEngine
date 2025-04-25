#pragma once

#include "GrappleECS/Entity/Component.h"
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

	struct GrappleECS_API ArchetypeRecord
	{
		size_t Id;
		int32_t DeletionQueryReferences;
		
		std::vector<ComponentId> Components; // Sorted
		std::vector<size_t> ComponentOffsets;

		std::unordered_map<ComponentId, ArchetypeEdge> Edges;

		ArchetypeRecord()
			: Id(INVALID_ARCHETYPE_ID), DeletionQueryReferences(0) {}

		ArchetypeRecord(const ArchetypeRecord&) = delete;

		ArchetypeRecord(ArchetypeRecord&& other) noexcept
			: Id(other.Id),
			Components(std::move(other.Components)),
			ComponentOffsets(std::move(other.ComponentOffsets)),
			Edges(std::move(other.Edges)),
			DeletionQueryReferences(other.DeletionQueryReferences)
		{
			other.Id = INVALID_ARCHETYPE_ID;
			other.DeletionQueryReferences = 0;
		}

		ArchetypeRecord& operator=(const ArchetypeRecord&) = delete;
		ArchetypeRecord& operator=(ArchetypeRecord&& other) noexcept
		{
			Id = other.Id;
			Components = std::move(other.Components);
			ComponentOffsets = std::move(other.ComponentOffsets);
			Edges = std::move(other.Edges);
			DeletionQueryReferences = other.DeletionQueryReferences;

			other.Id = INVALID_ARCHETYPE_ID;
			other.DeletionQueryReferences = 0;
			return *this;
		}

		constexpr bool IsUsedInDeletionQuery() const { return DeletionQueryReferences > 0; }
	};
}