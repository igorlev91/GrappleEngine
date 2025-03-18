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
}