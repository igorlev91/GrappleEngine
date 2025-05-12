#pragma once

#include "GrappleECS/Query/Query.h"

#include "GrappleECS/System/System.h"
#include "GrappleECS/System/SystemData.h"

#include "GrappleECS/Commands/CommandBuffer.h"

#include <functional>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Grapple
{
	class System;
	class GrappleECS_API World;
	class GrappleECS_API SystemsManager
	{
	public:
		SystemsManager(World& world);
		~SystemsManager();

		SystemGroupId CreateGroup(std::string_view name);
		std::optional<SystemGroupId> FindGroup(std::string_view name) const;

		SystemId RegisterSystem(std::string_view name, SystemGroupId group, System* system);
		SystemId RegisterSystem(std::string_view name, SystemGroupId group, const SystemEventFunction& onUpdate = nullptr, const SystemConfig* config = nullptr);
		SystemId RegisterSystem(std::string_view name, const SystemEventFunction& onUpdate = nullptr);

		void RegisterSystems(SystemGroupId defaultGroup);

		void AddSystemToGroup(SystemId system, SystemGroupId group);
		void AddSystemExecutionSettings(SystemId system, const std::vector<ExecutionOrder>* executionOrder);

		void ExecuteGroup(SystemGroupId id);
		bool IsGroupIdValid(SystemGroupId id);
		void RebuildExecutionGraphs();
		
		const std::vector<SystemGroup>& GetGroups() const;
		std::vector<SystemGroup>& GetGroups();

		const std::vector<SystemData>& GetSystems() const;
	private:
		World& m_World;
		CommandBuffer m_CommandBuffer;

		std::vector<SystemData> m_Systems;
		std::unordered_map<std::string, SystemGroupId> m_GroupNameToId;
		std::vector<SystemGroup> m_Groups;

		std::vector<System*> m_ManagedSystems;
	};
}