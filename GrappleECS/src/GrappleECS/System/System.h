#pragma once

#include "GrappleECS/Entity/Archetype.h"

#include "GrappleECS/Query/EntityView.h"
#include "GrappleECS/Query/QueryCache.h"
#include "GrappleECS/Query/Query.h"

#include <functional>

namespace Grapple
{
	using SystemFunction = std::function<void(EntityView)>;
	struct System
	{
		System(const Query& query, const SystemFunction& systemFunction)
			: SystemQuery(query), Archetype(INVALID_ARCHETYPE_ID), IsArchetypeQuery(false), Function(systemFunction) {}

		Query SystemQuery;
		ArchetypeId Archetype;
		bool IsArchetypeQuery;
		SystemFunction Function;
	};
}