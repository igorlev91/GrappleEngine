#pragma once

#include "GrappleECS/Registry.h"
#include "GrappleECS/Component.h"

#include <vector>
#include <string_view>

namespace Grapple
{
	class World
	{
	public:
		inline Registry& GetRegistry() { return m_Registry; }
	private:
		Registry m_Registry;
		std::vector<ComponentInfo> m_RegisteredComponents;
	};
}