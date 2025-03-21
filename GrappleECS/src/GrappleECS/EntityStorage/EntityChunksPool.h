#pragma once

#include "Grapple/Core/Core.h"

#include "GrappleECS/EntityStorage/EntityStorageChunk.h"

namespace Grapple
{
	class EntityChunksPool
	{
	public:
		EntityChunksPool(size_t capacity);

		EntityStorageChunk GetOrCreate();
		void Add(EntityStorageChunk& chunk);

		inline size_t GetCount() const { return m_Count; }
		inline size_t GetCapacity() const { return m_Capacity; }
	public:
		static void Initialize(size_t capacity);
		static Scope<EntityChunksPool>& GetInstance();
	private:
		size_t m_Capacity;
		size_t m_Count;

		EntityStorageChunk* m_Chunks;
	private:
		static Scope<EntityChunksPool> s_Instance;
	};
}