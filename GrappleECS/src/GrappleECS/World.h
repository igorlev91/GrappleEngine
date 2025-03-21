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
		template<typename ComponentT>
		void RegisterComponent()
		{
			ComponentT::Id = m_Registry.RegisterComponent(typeid(ComponentT).name(), sizeof(ComponentT), [](void* component) { ((ComponentT*)component)->~ComponentT(); });
		}
	public:
		inline Registry& GetRegistry() { return m_Registry; }

		void RegisterSystem(QueryId query, const SystemFunction& system);

		void OnUpdate();
	private:
		Registry m_Registry;

		std::vector<System> m_Systems;
	};
}