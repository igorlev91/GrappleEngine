#pragma once

#include "GrappleCore/Core.h"

namespace Grapple
{
	class Grapple_API ScriptingEngine
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void LoadModules();
		static void UnloadAllModules();
	};
}