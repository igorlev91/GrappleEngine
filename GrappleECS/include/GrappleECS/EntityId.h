#pragma once

#include <xhash>
#include <stdint.h>

namespace Grapple
{
	struct Entity
	{
	public:
		constexpr Entity()
			: m_Packed(SIZE_MAX) {}
		constexpr Entity(uint32_t id)
			: m_Packed(0), m_Index(id), m_Generation(0) {}
		constexpr Entity(uint32_t id, uint16_t generation)
			: m_Packed(0), m_Index(id), m_Generation(generation) {}

		constexpr uint32_t GetIndex() const { return m_Index; }
		constexpr uint16_t GetGeneration() const { return m_Generation; }

		constexpr bool operator==(Entity other) const
		{
			return m_Index == other.m_Index && m_Generation == other.m_Generation;
		}

		constexpr bool operator!=(Entity other) const
		{
			return m_Index != other.m_Index || m_Generation != other.m_Generation;
		}
	private:
		union
		{
			uint64_t m_Packed;
			struct
			{
				uint32_t m_Index;
				uint16_t m_Generation;
			};
		};

		friend struct std::hash<Entity>;
	};
}

template<>
struct std::hash<Grapple::Entity>
{

	size_t operator()(Grapple::Entity entity) const
	{
		std::hash<uint64_t> hashFunction;
		return hashFunction(entity.m_Index);
	}
};