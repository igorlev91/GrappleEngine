#pragma once

#include "GrappleCore/Assert.h"

#include "GrappleECS/Registry.h"

#include "GrappleECS/Query/ComponentView.h"
#include "GrappleECS/Query/EntityViewIterator.h"

#include <stdint.h>
#include <unordered_set>

namespace Grapple
{
	class GrappleECS_API EntityView
	{
	public:
		EntityView(Registry& registry, ArchetypeId archetype);
	public:
		EntityViewIterator begin();
		EntityViewIterator end();

		std::optional<Entity> GetEntity(size_t index);

		ArchetypeId GetArchetype() const;

		template<typename ComponentT>
		ComponentView<ComponentT> View()
		{
			ArchetypeRecord& archetypeRecord = m_Registry.GetArchetypeRecord(m_Archetype);
			std::optional<size_t> index = m_Registry.GetArchetypeComponentIndex(m_Archetype, COMPONENT_ID(ComponentT));

			Grapple_CORE_ASSERT(index.has_value(), "Archetype doesn't have a component");

			return ComponentView<ComponentT>(archetypeRecord.Data.ComponentOffsets[index.value()]);
		}
	private:
		Registry& m_Registry;
		ArchetypeId m_Archetype;
	};
}