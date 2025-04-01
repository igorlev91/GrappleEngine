#pragma once

#include "Grapple/Core/UUID.h"

#include "GrappleScripting/ScriptingModule.h"

#include "GrappleScriptingCore/ModuleConfiguration.h"

#include <filesystem>

namespace Grapple
{
	using ModuleEventFunction = void(*)(ModuleConfiguration&);

	struct ScriptingModuleData
	{
		ModuleConfiguration Config;
		Ref<ScriptingModule> Module;

		std::optional<ModuleEventFunction> OnLoad;
		std::optional<ModuleEventFunction> OnUnload;
	};

	class ScriptingEngine
	{
	private:
		struct Data
		{
			std::vector<ScriptingModuleData> Modules;
		};
	public:
		static void Initialize();
		static void Shutdown();

		static void ReloadModules();

		static void LoadModule(const std::filesystem::path& modulePath);
		static void UnloadAllModules();
	private:
		static Data s_Data;

		static const std::string s_ModuleLoaderFunctionName;
		static const std::string s_ModuleUnloaderFunctionName;
	};
}