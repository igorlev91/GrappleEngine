#pragma once

#include "GrappleECS/Component.h"
#include "GrappleECS/EntityStorage.h"

#include <vector>
#include <optional>

namespace Grapple
{
	struct Archetype
	{
		size_t Id;
		std::vector<ComponentId> Components; // Sorted
		std::vector<size_t> ComponentOffsets;

		std::optional<size_t> FindComponent(ComponentId component);
	};
	
	struct ArchetypeRecord
	{
		Archetype Data;
		EntityStorage Storage;
	};
}