#pragma once

#include "GrappleCore/UUID.h"
#include "GrappleECS/World.h"

#include <filesystem>

namespace Grapple
{
	class Grapple_API ScriptingEngine
	{
	public:
		struct Data
		{
			World* CurrentWorld = nullptr;
			std::vector<void*> LoadedSharedLibraries;
		};
	public:
		static void Initialize();
		static void Shutdown();

		static void SetCurrentECSWorld(World& world);

		static void LoadModules();

		static void UnloadAllModules();
		static void RegisterSystems();
		
		static Data& GetData();
	};
}