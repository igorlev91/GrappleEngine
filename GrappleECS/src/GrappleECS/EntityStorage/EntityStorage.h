#pragma once

#include "GrappleCore/Assert.h"

#include "GrappleECS/EntityStorage/EntityStorageChunk.h"

#include <stdint.h>

namespace Grapple
{
	class GrappleECS_API EntityStorage
	{
	public:
		EntityStorage();
		EntityStorage(EntityStorage&& other) noexcept;

		size_t AddEntity(uint32_t registryIndex);
		uint8_t* GetEntityData(size_t entityIndex) const;

		void RemoveEntityData(size_t entityIndex);
		
		size_t GetEntitiesCount() const;
		size_t GetEntitySize() const;
		void SetEntitySize(size_t entitySize);
		void UpdateEntityRegistryIndex(size_t entityIndex, uint32_t newRegistryIndex);

		size_t GetChunksCount() const;
		size_t GetEntitiesCountInChunk(size_t index) const;

		uint8_t* GetChunkBuffer(size_t index);
		const uint8_t* GetChunkBuffer(size_t index) const;
		const std::vector<uint32_t>& GetEntityIndices() const;
	private:
		std::vector<EntityStorageChunk> m_Chunks;
		std::vector<uint32_t> m_EntityIndices;

		size_t m_EntitySize;
		size_t m_EntitiesCount;
		size_t m_EntitiesPerChunk;
	};
}