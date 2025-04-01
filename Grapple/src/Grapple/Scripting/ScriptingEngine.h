#pragma once

#include "Grapple/Core/UUID.h"
#include "Grapple/Scripting/ScriptingModule.h"

#include "GrappleECS/World.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"
#include "GrappleScriptingCore/SystemInfo.h"

#include <filesystem>

namespace Grapple
{
	using ModuleEventFunction = void(*)(ModuleConfiguration&);

	struct ScriptingTypeInstance
	{
		size_t TypeIndex = SIZE_MAX;
		void* Instance = nullptr;
	};

	struct ScriptingModuleData
	{
		ModuleConfiguration Config;
		Ref<ScriptingModule> Module;

		std::optional<ModuleEventFunction> OnLoad;
		std::optional<ModuleEventFunction> OnUnload;

		std::vector<ScriptingTypeInstance> ScriptingInstances;
		std::unordered_map<std::string_view, size_t> TypeNameToIndex;
	};

	class ScriptingEngine
	{
	public:
		struct Data
		{
			World* CurrentWorld = nullptr;
			std::vector<ScriptingModuleData> Modules;
		};
	public:
		static void Initialize();
		static void Shutdown();

		static void SetCurrentECSWorld(World& world);

		static void ReloadModules();
		static void ReleaseScriptingInstances();

		static void LoadModule(const std::filesystem::path& modulePath);
		static void UnloadAllModules();

		static void RegisterSystems();

		inline static Data& GetData() { return s_Data; }
	private:
		static Data s_Data;

		static const std::string s_ModuleLoaderFunctionName;
		static const std::string s_ModuleUnloaderFunctionName;
	};
}