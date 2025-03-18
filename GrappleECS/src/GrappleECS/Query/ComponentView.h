#pragma once

#include "GrappleECS/Registry.h"
#include "GrappleECS/Query/EntityViewIterator.h"

namespace Grapple
{
	template<typename ComponentT>
	class ComponentView
	{
	public:
		constexpr ComponentView(size_t offset)
			: m_ComponentOffset(offset) {}

		constexpr ComponentT& operator[](EntityViewElement& entity)
		{
			return *(ComponentT*)(entity.GetEntityData() + m_ComponentOffset);
		}
	private:
		size_t m_ComponentOffset;
	};
}