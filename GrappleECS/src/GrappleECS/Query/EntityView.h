#pragma once

#include "GrappleCore/Assert.h"

#include "GrappleECS/Entities.h"

#include "GrappleECS/Query/ComponentView.h"
#include "GrappleECS/Query/EntityViewIterator.h"

#include <stdint.h>
#include <unordered_set>

namespace Grapple
{
	class GrappleECS_API EntityView
	{
	public:
		EntityView(Entities& entities, ArchetypeId archetype);
	public:
		EntityViewIterator begin();
		EntityViewIterator end();

		std::optional<Entity> GetEntity(size_t index);

		ArchetypeId GetArchetype() const;

		template<typename ComponentT>
		ComponentView<ComponentT> View()
		{
			const ArchetypeRecord& archetypeRecord = m_Entities.GetArchetypes()[m_Archetype];
			std::optional<size_t> index = m_Entities.GetArchetypes().GetArchetypeComponentIndex(m_Archetype, COMPONENT_ID(ComponentT));

			Grapple_CORE_ASSERT(index.has_value(), "Archetype doesn't have a component");

			return ComponentView<ComponentT>(archetypeRecord.ComponentOffsets[index.value()]);
		}
	private:
		Entities& m_Entities;
		ArchetypeId m_Archetype;
	};
}