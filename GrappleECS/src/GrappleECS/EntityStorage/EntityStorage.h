#pragma once

#include "Grapple/Core/Assert.h"

#include "GrappleECS/EntityStorage/EntityStorageChunk.h"

#include <stdint.h>

namespace Grapple
{
	class EntityStorage
	{
	public:
		EntityStorage();
		EntityStorage(EntityStorage&& other) noexcept;

		size_t AddEntity(size_t registryIndex);
		uint8_t* GetEntityData(size_t entityIndex) const;

		void RemoveEntityData(size_t entityIndex);
		
		inline size_t GetEntitiesCount() const { return m_EntitiesCount; }
		inline size_t GetEntitySize() const { return m_EntitySize; }
		void SetEntitySize(size_t entitySize);
		void UpdateEntityRegistryIndex(size_t entityIndex, size_t newRegistryIndex);

		inline size_t GetChunksCount() const { return m_Chunks.size(); }
		inline size_t GetEntitiesCountInChunk(size_t index) const
		{
			Grapple_CORE_ASSERT(index < m_Chunks.size());
			if (index == m_Chunks.size() - 1)
				return m_EntitiesCount % m_EntitiesPerChunk;
			return m_EntitiesPerChunk;
		}

		inline uint8_t* GetChunkBuffer(size_t index)
		{
			Grapple_CORE_ASSERT(index < m_Chunks.size());
			return m_Chunks[index].GetBuffer();
		}

		inline const std::vector<size_t>& GetEntityIndices() const { return m_EntityIndices; }
	private:
		std::vector<EntityStorageChunk> m_Chunks;
		std::vector<size_t> m_EntityIndices;

		size_t m_EntitySize;
		size_t m_EntitiesCount;
		size_t m_EntitiesPerChunk;
	};
}