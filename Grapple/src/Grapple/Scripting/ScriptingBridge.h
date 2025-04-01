#pragma once

#include "GrappleECS/World.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"

namespace Grapple
{
	class ScriptingBridge
	{
	public:
		static void ConfigureModule(ModuleConfiguration& config);

		inline static void SetCurrentWorld(World& world)
		{
			s_World = &world;
		}
	private:
		static World* s_World;
	};
}