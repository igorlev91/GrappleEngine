#include "EntityStorage.h"

namespace Grapple
{
	size_t EntityStorage::AddEntity()
	{
		Grapple_CORE_ASSERT(m_EntitySize > 0, "Entity has no size");

		if (!m_Chunk.IsAllocated())
			m_Chunk.Allocate();

		size_t index = m_EntitiesCount;
		size_t offset = m_EntitySize * m_EntitiesCount;

		Grapple_CORE_ASSERT(offset < m_Chunk.GetSize());
		Grapple_CORE_ASSERT(offset + m_EntitySize <= m_Chunk.GetSize());

		m_EntitiesCount++;
		return index;
	}

	inline uint8_t* EntityStorage::GetEntityData(size_t entityIndex)
	{
		size_t bytesOffset = entityIndex * m_EntitySize;

		Grapple_CORE_ASSERT(m_Chunk.IsAllocated());
		Grapple_CORE_ASSERT(bytesOffset < m_Chunk.GetSize());

		return m_Chunk.GetBuffer() + bytesOffset;
	}

	void EntityStorage::SetEntitySize(size_t entitySize)
	{
		Grapple_CORE_ASSERT(!(m_Chunk.IsAllocated() && m_EntitiesCount > 0), "Entity size can only be set if the storage is empty");
		m_EntitySize = entitySize;
	}
}