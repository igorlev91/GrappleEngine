#pragma once

#include "GrappleECS/EntityId.h"
#include "GrappleECS/Entity/Archetype.h"

#include <stdint.h>
#include <xhash>

namespace Grapple
{
	struct EntityRecord
	{
		Entity Id;

		uint32_t RegistryIndex;
		ArchetypeId Archetype;
		size_t BufferIndex;
	};
}
