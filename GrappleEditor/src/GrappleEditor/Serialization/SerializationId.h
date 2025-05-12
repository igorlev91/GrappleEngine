#pragma once

#include "GrappleCore/UUID.h"
#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore/Serialization/SerializationStream.h"

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

	template<>
	struct TypeSerializer<SerializationId>
	{
		void OnSerialize(SerializationId& id, SerializationStream& stream)
		{
			stream.Serialize("Id", SerializationValue(id.Id));
		}
	};
}