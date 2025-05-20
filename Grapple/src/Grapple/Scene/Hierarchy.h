#pragma once

#include "GrappleCore/Serialization/TypeSerializer.h"
#include "GrappleCore/Serialization/SerializationStream.h"

#include "GrappleECS/World.h"
#include "GrappleECS/Entity/ComponentInitializer.h"

namespace Grapple
{
	struct Grapple_API Children
	{
		Grapple_COMPONENT;

		std::vector<Entity> ChildrenEntities;
	};

	template<>
	struct TypeSerializer<Children>
	{
		static void OnSerialize(Children& children, SerializationStream& stream)
		{
			stream.Serialize("ChildrenEntities", SerializationValue(children.ChildrenEntities));
		}
	};



	struct Grapple_API Parent
	{
		Grapple_COMPONENT;

		Parent()
			: ParentEntity(Entity()), IndexInParent(0) {}

		Entity ParentEntity;
		uint32_t IndexInParent;
	};

	template<>
	struct TypeSerializer<Parent>
	{
		static void OnSerialize(Parent& parent, SerializationStream& stream)
		{
			stream.Serialize("ParentEntity", SerializationValue(parent.ParentEntity));
			stream.Serialize("IndexInParent", SerializationValue(parent.IndexInParent));
		}
	};
}
