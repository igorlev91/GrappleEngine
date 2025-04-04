#include "EntityView.h"

namespace Grapple
{
	EntityViewIterator EntityView::begin()
	{
		return EntityViewIterator(m_Registry.GetArchetypeRecord(m_Archetype).Storage, 0);
	}

	EntityViewIterator EntityView::end()
	{
		EntityStorage& storage = m_Registry.GetArchetypeRecord(m_Archetype).Storage;
		return EntityViewIterator(storage, storage.GetEntitiesCount());
	}

	std::optional<Entity> EntityView::GetEntity(uint32_t index)
	{
		const auto& indices = m_Registry.GetArchetypeRecord(m_Archetype).Storage.GetEntityIndices();
		Grapple_CORE_ASSERT(index < indices.size());

		return m_Registry.FindEntityByRegistryIndex(indices[index]);
	}
}