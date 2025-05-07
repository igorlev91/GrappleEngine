#pragma once

#include "GrappleCore/UUID.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/ComponentInitializer.h"

namespace Grapple
{
	struct SerializationId
	{
		Grapple_COMPONENT;

		SerializationId() = default;
		SerializationId(UUID id)
			: Id(id) {}

		UUID Id;
	};
}