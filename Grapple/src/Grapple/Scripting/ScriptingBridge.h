#pragma once

#include "GrappleECS/World.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"

namespace Grapple
{
	class ScriptingBridge
	{
	public:
		static void Initialize();

		inline static World& GetCurrentWorld();
		inline static Scripting::Bindings& GetBindings() { return s_Bindings; }

		inline static void SetCurrentWorld(World& world)
		{
			s_World = &world;
		}
	private:
		static World* s_World;
		static Scripting::Bindings s_Bindings;
	};
}