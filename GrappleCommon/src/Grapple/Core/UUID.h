#pragma once

#include <xhash>

namespace Grapple
{
	class UUID
	{
	public:
		UUID();
		constexpr UUID(uint64_t value)
			: m_Value(value) {}

		inline operator uint64_t() const { return m_Value; }
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