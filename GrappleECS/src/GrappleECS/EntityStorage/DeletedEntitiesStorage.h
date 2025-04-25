#pragma once

#include "GrappleECS/Entity/Entity.h"
#include "GrappleECS/EntityStorage/EntityStorage.h"

namespace Grapple
{
	struct DeletedEntitiesStorage
	{
		inline void Clear()
		{
			DataStorage.Clear();
			Ids.clear();
		}

		EntityDataStorage DataStorage;
		std::vector<Entity> Ids;
	};
}