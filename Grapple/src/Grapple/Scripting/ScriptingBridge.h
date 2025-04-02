#pragma once

#include "GrappleECS/World.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"

namespace Grapple
{
	class ScriptingBridge
	{
	public:
		static void ConfigureModule(Internal::ModuleConfiguration& config);

		inline static World& GetCurrentWorld();

		inline static void SetCurrentWorld(World& world)
		{
			s_World = &world;
		}
	private:
		static World* s_World;
	};
}