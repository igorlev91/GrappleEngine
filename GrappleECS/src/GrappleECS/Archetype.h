#pragma once

#include "GrappleECS/Component.h"
#include "GrappleECS/EntityStorage.h"

#include <vector>

namespace Grapple
{
	struct Archetype
	{
		size_t Id;
		std::vector<ComponentId> Components; // Sorted
		std::vector<size_t> ComponentOffsets;
	};
	
	struct ArchetypeRecord
	{
		Archetype Data;
		EntityStorage Storage;
	};
}