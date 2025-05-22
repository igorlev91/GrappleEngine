#pragma once

#include "GrappleECS/Entity/Component.h"
#include "GrappleECS/Entity/Archetype.h"

#include "GrappleECS/Query/QueryCache.h"
#include "GrappleECS/Query/QueryData.h"
#include "GrappleECS/Query/EntityView.h"

#include "GrappleECS/Entities.h"

#include <vector>
#include <unordered_set>

namespace Grapple
{
	class QueryIterator
	{
	public:
		QueryIterator(Entities& entities, QueryTarget target, const std::unordered_set<ArchetypeId>::const_iterator& archetype)
			: m_Entities(entities), m_Target(target), m_Archetype(archetype) {}

		inline EntityView operator*()
		{
			return EntityView(m_Entities, m_Target, *m_Archetype);
		}

		inline QueryIterator operator++()
		{
			m_Archetype++;
			return *this;
		}

		inline bool operator==(const QueryIterator& other)
		{
			return &m_Entities == &other.m_Entities && m_Archetype == other.m_Archetype;
		}

		inline bool operator!=(const QueryIterator& other)
		{
			return &m_Entities != &other.m_Entities || m_Archetype != other.m_Archetype;
		}
	private:
		Entities& m_Entities;
		QueryTarget m_Target;
		std::unordered_set<ArchetypeId>::const_iterator m_Archetype;
	};

	template<typename T>
	using QueryIteratorParamterType = typename T;

	template<typename T>
	inline T GetQueryIteratorParameterValue(size_t entityIndex, size_t componentOffset, uint8_t* entityData, const EntityStorage& storage)
	{
		return *(std::remove_reference_t<T>*)(entityData + componentOffset);
	}

	template<>
	inline Entity GetQueryIteratorParameterValue<Entity>(size_t entityIndex, size_t componentOffset, uint8_t* entityData, const EntityStorage& storage)
	{
		// TODO: implement
		return Entity();
	}

	class QueryChunkIterator
	{
	public:
		QueryChunkIterator(uint8_t* entityData, size_t entitySize)
			: m_EntityData(entityData), m_EntitySize(entitySize) {}

		inline EntityViewElement operator*() { return EntityViewElement(m_EntityData); }

		inline QueryChunkIterator& operator++()
		{
			m_EntityData += m_EntitySize;
			return *this;
		}

		inline bool operator==(const QueryChunkIterator& other)
		{
			return m_EntityData == other.m_EntityData;
		}

		inline bool operator!=(const QueryChunkIterator& other)
		{
			return m_EntityData != other.m_EntityData;
		}
	private:
		uint8_t* m_EntityData;
		size_t m_EntitySize;
	};

	class QueryChunk
	{
	public:
		QueryChunk(uint8_t* chunkData, size_t entitiesCount, size_t entitySize)
			: m_ChunkData(chunkData), m_EntitiesCount(entitiesCount), m_EntitySize(entitySize) {}

		inline QueryChunkIterator begin() const { return QueryChunkIterator(m_ChunkData, m_EntitySize); }
		inline QueryChunkIterator end() const { return QueryChunkIterator(m_ChunkData + m_EntitySize * m_EntitiesCount, m_EntitySize); }
	private:
		uint8_t* m_ChunkData;
		size_t m_EntitiesCount;
		size_t m_EntitySize;
	};

	class GrappleECS_API Query
	{
	public:
		constexpr Query()
			: m_Id(INVALID_QUERY_ID), m_Entities(nullptr), m_QueryCache(nullptr) {}
		constexpr Query(QueryId id, Entities& entities, const QueryCache& queries)
			: m_Id(id), m_Entities(&entities), m_QueryCache(&queries) {}
	public:
		QueryId GetId() const;

		QueryIterator begin() const;
		QueryIterator end() const;
		const std::unordered_set<ArchetypeId>& GetMatchedArchetypes() const;

		size_t GetEntitiesCount() const;

		template<typename... Args, typename IteratorFunction>
		inline void ForEachEntity(const IteratorFunction& function)
		{
			size_t componentOffsets[sizeof...(Args)];
			const QueryData& data = (*m_QueryCache)[m_Id];
			const Archetypes& archetypes = m_Entities->GetArchetypes();
			for (ArchetypeId matchedArchetype : data.MatchedArchetypes)
			{
				EntityStorage& storage = m_Entities->GetEntityStorage(matchedArchetype);
				const ArchetypeRecord& archetype = archetypes[matchedArchetype];

				size_t index = 0;
				([&]()
				{
					std::optional<size_t> componentIndex = archetypes.GetArchetypeComponentIndex(matchedArchetype, COMPONENT_ID(std::remove_reference_t<Args>));
					if (componentIndex)
						componentOffsets[index] = archetype.ComponentOffsets[*componentIndex];
					index++;
				} (), ...);

				for (size_t chunkIndex = 0; chunkIndex < storage.GetChunksCount(); chunkIndex++)
				{
					uint8_t* entityData = storage.GetChunkBuffer(chunkIndex);
					for (size_t entityIndex = 0; entityIndex < storage.GetEntitiesCountInChunk(chunkIndex); entityIndex++)
					{
						size_t componentIndex = 0;

						std::tuple<QueryIteratorParamterType<Args>...> arguments = std::make_tuple(
							[&]() -> decltype(auto)
							{
								return GetQueryIteratorParameterValue<Args>(entityIndex, componentOffsets[componentIndex++], entityData, storage);
							} () ...
						);

						std::apply(function, arguments);
						entityData += storage.GetEntitySize();
					}
				}
			}
		}

		template<typename... Args, typename IteratorFunction>
		inline void ForEachChunk(const IteratorFunction& function)
		{
			size_t componentOffsets[sizeof...(Args)];
			const QueryData& data = (*m_QueryCache)[m_Id];
			const Archetypes& archetypes = m_Entities->GetArchetypes();
			for (ArchetypeId matchedArchetype : data.MatchedArchetypes)
			{
				EntityStorage& storage = m_Entities->GetEntityStorage(matchedArchetype);
				const ArchetypeRecord& archetype = archetypes[matchedArchetype];

				size_t index = 0;
				([&]()
				{
					std::optional<size_t> componentIndex = archetypes.GetArchetypeComponentIndex(matchedArchetype, COMPONENT_ID(std::remove_reference_t<Args>));
					if (componentIndex)
						componentOffsets[index] = archetype.ComponentOffsets[*componentIndex];
					index++;
				} (), ...);

				for (size_t chunkIndex = 0; chunkIndex < storage.GetChunksCount(); chunkIndex++)
				{
					uint8_t* entityData = storage.GetChunkBuffer(chunkIndex);
					size_t componentIndex = 0;

					std::tuple<QueryChunk, ComponentView<Args>...> arguments = std::make_tuple(
						QueryChunk(entityData, storage.GetEntitiesCountInChunk(chunkIndex), storage.GetEntitySize()),
						[&]() -> decltype(auto)
						{
							return ComponentView<Args>(componentOffsets[componentIndex++]);
						} () ...
					);

					std::apply(function, arguments);
				}
			}
		}
	private:
		QueryId m_Id;
		Entities* m_Entities;
		const QueryCache* m_QueryCache;
	};
}