#pragma once

#include "GrappleECS/System/SystemData.h"

namespace Grapple
{
	class GrappleECS_API World;
	struct SystemConfig
	{
		std::optional<SystemGroupId> Group;
		World* world;
		int* a = nullptr;
	};

	class System
	{
	public:
		virtual ~System() {}

		virtual void OnConfig(SystemConfig& config) = 0;
		virtual void OnUpdate(SystemExecutionContext& context) = 0;
	};
}