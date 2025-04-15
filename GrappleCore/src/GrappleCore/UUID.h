#pragma once

#include "GrappleCore/Core.h"

#include <xhash>

namespace Grapple
{
	class GrappleCORE_API UUID
	{
	public:
		UUID();
		constexpr UUID(uint64_t value)
			: m_Value(value) {}
		
		constexpr operator uint64_t() const { return m_Value; }
		constexpr bool operator==(UUID other) const { return m_Value == other.m_Value; }
		constexpr bool operator!=(UUID other) const { return m_Value != other.m_Value; }
		constexpr UUID& operator=(UUID other) { m_Value = other.m_Value; return *this; }
		constexpr UUID& operator=(uint64_t value) { m_Value = value; return *this; }
	private:
		uint64_t m_Value;
		
		friend struct std::hash<UUID>;
	};
}

template<>
struct std::hash<Grapple::UUID>
{
	size_t operator()(Grapple::UUID uuid) const
	{
		return std::hash<uint64_t>()(uuid.m_Value);
	}
};