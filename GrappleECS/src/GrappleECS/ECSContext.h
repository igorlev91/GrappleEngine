#pragma once

#include "GrappleECS/Entity/Archetypes.h"
#include "GrappleECS/Entity/Components.h"

namespace Grapple
{
	struct ECSContext
	{
		inline void Clear()
		{
			Archetypes.Clear();
			Components.Clear();
		}

		Grapple::Archetypes Archetypes;
		Grapple::Components Components;
	};
}