#pragma once

#include "Grapple/Core/Assert.h"

#include "GrappleECS/Registry.h"

#include "GrappleECS/Query/ComponentView.h"
#include "GrappleECS/Query/EntityViewIterator.h"

#include <stdint.h>

namespace Grapple
{
	class EntityView
	{
	public:
		EntityView(Registry& registry, size_t archetype)
			: m_Registry(registry), m_Archetype(archetype) {}
	public:
		EntityViewIterator begin();
		EntityViewIterator end();

		template<typename ComponentT>
		ComponentView<ComponentT> View()
		{
			ArchetypeRecord& archetypeRecord = m_Registry.GetArchetypeRecord(m_Archetype);
			std::optional<size_t> index = archetypeRecord.Data.FindComponent(ComponentT::Id);

			Grapple_CORE_ASSERT(index.has_value(), "Archetype doesn't have a component");

			return ComponentView<ComponentT>(archetypeRecord.Data.ComponentOffsets[index.value()]);
		}
	private:
		Registry& m_Registry;
		size_t m_Archetype;
	};
}