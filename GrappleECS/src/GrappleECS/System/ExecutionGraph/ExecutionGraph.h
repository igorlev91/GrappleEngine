#pragma once

#include "GrappleECS/System.h"

#include <vector>
#include <unordered_set>
#include <stdint.h>

namespace Grapple
{	
	class ExecutionGraph
	{
	public:
		struct GraphNode
		{
			std::vector<uint32_t> Dependecies;
			std::vector<uint32_t> Children;
		};

		enum class VisitedFlag : uint8_t
		{
			None = 0,
			Visited = 1,
			InCurrentPath = 2
		};

		enum class BuildResult
		{
			Success,
			CircularDependecy,
		};
	public:
		void AddExecutionSettings();
		void AddExecutionSettings(const std::vector<ExecutionOrder> settings);

		inline const std::vector<uint32_t>& GetExecutionOrder() const { return m_ExecutionOrder; }
	
		BuildResult RebuildGraph();
	private:
		void GenerateExecutionOrderList(uint32_t initialNode, std::unordered_set<uint32_t>& unresolvedNodes);
		bool CheckForCicularDependecies();
		bool CheckForCicularDependecies(uint32_t node);
		bool HasIncompleteDependecies(uint32_t index);
	private:
		std::vector<std::vector<ExecutionOrder>> m_ExecutionSettings;
		std::vector<uint32_t> m_ExecutionOrder;
		std::vector<VisitedFlag> m_Visited;
		std::vector<GraphNode> m_Graph;
	};
}