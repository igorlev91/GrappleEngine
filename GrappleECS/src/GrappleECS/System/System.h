#pragma once

#include "GrappleECS/Archetype.h"

#include "GrappleECS/Query/EntityView.h"
#include "GrappleECS/Query/QueryCache.h"

#include <functional>

namespace Grapple
{
	using SystemFunction = std::function<void(EntityView)>;
	struct System
	{
		QueryId Query;
		ArchetypeId Archetype;
		bool IsArchetypeQuery;
		SystemFunction Function;
	};
}