#include "Archetypes.h"

namespace Grapple
{
	Archetypes::~Archetypes()
	{
		for (const auto& archetype : Records)
		{
			Grapple_CORE_ASSERT(archetype.DeletionQueryReferences == 0 && archetype.CreatedEntitiesQueryReferences == 0);
		}
	}
}
