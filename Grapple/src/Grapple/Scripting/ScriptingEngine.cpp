#include "ScriptingEngine.h"

#include "Grapple/Core/Log.h"

#include "Grapple/Scene/Components.h"
#include "Grapple/Project/Project.h"

#include "GrapplePlatform/Platform.h"

#include "GrappleECS/System/SystemInitializer.h"

#include "GrappleECS.h"

namespace Grapple
{
	const std::string s_ModuleLoaderFunctionName = "OnModuleLoaded";
	const std::string s_ModuleUnloaderFunctionName = "OnModuleUnloaded";

	ScriptingEngine::Data s_Data;

	void ScriptingEngine::Initialize()
	{
	}

	void ScriptingEngine::Shutdown()
	{
		UnloadAllModules();
	}

	void ScriptingEngine::SetCurrentECSWorld(World& world)
	{
		s_Data.CurrentWorld = &world;
	}

	void ScriptingEngine::LoadModules()
	{
		Grapple_CORE_ASSERT(Project::GetActive());

		std::string_view configurationName = "";
		std::string_view platformName = "";
#ifdef Grapple_DEBUG
		configurationName = "Debug";
#elif defined(Grapple_RELEASE)
		configurationName = "Release";
#elif defined(Grapple_DIST)
		configurationName = "Dist";
#endif

#ifdef Grapple_PLATFORM_WINDOWS
		platformName = "windows";
#endif
		
		for (const std::string& moduleName : Project::GetActive()->ScriptingModules)
		{
			std::filesystem::path libraryPath = Project::GetActive()->Location
				/ fmt::format("bin/{0}-{1}-x86_64/{2}.dll",
					configurationName,
					platformName,
					moduleName);

			void* library = Platform::LoadSharedLibrary(libraryPath);
			Grapple_CORE_ASSERT(library);

			s_Data.LoadedSharedLibraries.push_back(library);
		}
	}

	void ScriptingEngine::UnloadAllModules()
	{
		for (void* lib : s_Data.LoadedSharedLibraries)
			Platform::FreeSharedLibrary(lib);
		s_Data.LoadedSharedLibraries.clear();
	}

	void ScriptingEngine::RegisterSystems()
	{
		std::string_view defaultGroupName = "Scripting Update";
		std::optional<SystemGroupId> defaultGroup = s_Data.CurrentWorld->GetSystemsManager().FindGroup(defaultGroupName);
		Grapple_CORE_ASSERT(defaultGroup.has_value());

		s_Data.CurrentWorld->GetSystemsManager().RegisterSystems(defaultGroup.value());
	}

	ScriptingEngine::Data& ScriptingEngine::GetData()
	{
		return s_Data;
	}
}