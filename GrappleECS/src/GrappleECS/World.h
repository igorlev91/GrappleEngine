#pragma once

#include "GrappleECS/Registry.h"
#include "GrappleECS/Entity/Component.h"

#include "GrappleECS/System/System.h"

#include <vector>
#include <string_view>

namespace Grapple
{
	class World
	{
	public:
		inline Registry& GetRegistry() { return m_Registry; }

		void RegisterSystem(QueryId query, const SystemFunction& system);

		void OnUpdate();
	private:
		Registry m_Registry;

		std::vector<System> m_Systems;
	};
}