#include "SystemsManager.h"

namespace Grapple
{
	SystemsManager::~SystemsManager()
	{
		for (System* system : m_ManagedSystems)
			delete system;
	}

	SystemGroupId SystemsManager::CreateGroup(std::string_view name)
	{
		SystemGroupId id = (SystemGroupId)m_Groups.size();
		SystemGroup& group = m_Groups.emplace_back();
		group.Id = id;
		group.Name = name;

		m_GroupNameToId.emplace(group.Name, id);
		return id;
	}

	std::optional<SystemGroupId> SystemsManager::FindGroup(std::string_view name) const
	{
		auto it = m_GroupNameToId.find(std::string(name));
		if (it == m_GroupNameToId.end())
			return {};
		return it->second;
	}

	SystemId SystemsManager::RegisterSystem(std::string_view name, SystemGroupId group, System* system)
	{
		SystemId id = RegisterSystem(name, group, [system](SystemExecutionContext& context)
		{
			system->OnUpdate(context);
		});

		m_ManagedSystems.push_back(system);
		return id;
	}

	SystemId SystemsManager::RegisterSystem(std::string_view name, SystemGroupId group,
		const SystemEventFunction& onUpdate)
	{
		SystemId id = RegisterSystem(name, onUpdate);
		AddSystemToGroup(id, group);
		AddSystemExecutionSettings(id, nullptr);

		return id;
	}

	SystemId SystemsManager::RegisterSystem(std::string_view name, 
		const SystemEventFunction& onUpdate)
	{
		SystemId id = (SystemId) m_Systems.size();

		SystemData& data = m_Systems.emplace_back();
		data.Name = name;
		data.Id = id;
		data.GroupId = UINT32_MAX;
		data.OnUpdate = onUpdate;
		
		return id;
	}

	void SystemsManager::AddSystemToGroup(SystemId system, SystemGroupId group)
	{
		Grapple_CORE_ASSERT(m_Systems[system].GroupId == UINT32_MAX, "System is already assigned to a group");

		SystemData& data = m_Systems[system];

		m_Groups[group].SystemIndices.push_back(data.Id);

		data.GroupId = group;
		data.IndexInGroup = (uint32_t)m_Groups[group].SystemIndices.size() - 1;
	}

	void SystemsManager::AddSystemExecutionSettings(SystemId system, const std::vector<ExecutionOrder>* executionOrder)
	{
		SystemData& data = m_Systems[system];

		if (executionOrder == nullptr)
			m_Groups[data.GroupId].Graph.AddExecutionSettings();
		else
		{
			std::vector<ExecutionOrder> order = *executionOrder;
			for (auto& i : order)
				i.ItemIndex = m_Systems[i.ItemIndex].IndexInGroup;

			m_Groups[data.GroupId].Graph.AddExecutionSettings(std::move(order));
		}
	}

	void SystemsManager::ExecuteGroup(SystemGroupId id)
	{
		Grapple_CORE_ASSERT(id < (SystemGroupId)m_Groups.size());

		SystemGroup& group = m_Groups[id];
		for (size_t i : group.Graph.GetExecutionOrder())
		{
			SystemData& data = m_Systems[group.SystemIndices[i]];
			
			if (data.OnUpdate)
				data.OnUpdate(data.ExecutionContext);
		}
	}

	bool SystemsManager::IsGroupIdValid(SystemGroupId id)
	{
		return id < (SystemGroupId)m_Groups.size();
	}

	void SystemsManager::RebuildExecutionGraphs()
	{
		for (SystemGroup& group : m_Groups)
		{
			if (group.Graph.RebuildGraph() == ExecutionGraph::BuildResult::CircularDependecy)
			{
				Grapple_CORE_ERROR("Faield to build an execution graph for '{0}' because of circular dependecy", group.Name);
				continue;
			}
		}
	}

	const std::vector<SystemGroup>& SystemsManager::GetGroups() const
	{
		return m_Groups;
	}

	std::vector<SystemGroup>& SystemsManager::GetGroups()
	{
		return m_Groups;
	}

	const std::vector<SystemData>& SystemsManager::GetSystems() const
	{
		return m_Systems;
	}
}
