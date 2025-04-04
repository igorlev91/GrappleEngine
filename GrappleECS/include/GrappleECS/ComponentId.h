#pragma once

#include <xhash>
#include <stdint.h>

namespace Grapple
{
	struct ComponentId
	{
	public:
		constexpr ComponentId()
			: m_Index(UINT32_MAX), m_Generation(UINT16_MAX) {}
		constexpr ComponentId(uint32_t index, uint16_t generation)
			: m_Index(index), m_Generation(generation) {}

		constexpr uint32_t GetIndex() const { return m_Index; }
		constexpr uint16_t GetGeneration() const { return m_Generation; }

		constexpr bool ComapreMasked(ComponentId other) const
		{
			return (m_Index & INDEX_MASK) == (other.m_Index & INDEX_MASK) && m_Generation == other.m_Generation;
		}

		constexpr bool operator<(ComponentId other) const
		{
			return m_Index < other.m_Index;
		}

		constexpr bool operator>(ComponentId other) const
		{
			return m_Index > other.m_Index;
		}

		constexpr bool operator==(ComponentId other) const
		{
			return m_Index == other.m_Index && m_Generation == other.m_Generation;
		}

		constexpr bool operator!=(ComponentId other) const
		{
			return m_Index != other.m_Index || m_Generation != other.m_Generation;
		}

		static constexpr uint32_t INDEX_MASK = 0xffffffff >> 4;
	private:
		uint32_t m_Index;
		uint16_t m_Generation;

		friend struct std::hash<ComponentId>;
	};

	constexpr ComponentId INVALID_COMPONENT_ID = ComponentId();
}

template<>
struct std::hash<Grapple::ComponentId>
{
	size_t operator()(Grapple::ComponentId id) const
	{
		return std::hash<uint32_t>()(id.m_Index);
	}
};