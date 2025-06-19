#include "ScriptingEngine.h"

#include "GrappleCore/Log.h"

#include "Grapple/Project/Project.h"

#include "GrapplePlatform/Platform.h"

#include <vector>

namespace Grapple
{
	struct ScriptingEngineData
	{
		std::vector<void*> LoadedSharedLibraries;
	};

	ScriptingEngineData s_ScriptingData;

	void ScriptingEngine::Initialize()
	{
	}

	void ScriptingEngine::Shutdown()
	{
		UnloadAllModules();
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
			if (!library)
			{
				Grapple_CORE_ERROR("Failed to load module '{0}'", moduleName);
				continue;
			}

			s_ScriptingData.LoadedSharedLibraries.push_back(library);
		}
	}

	void ScriptingEngine::UnloadAllModules()
	{
		for (void* lib : s_ScriptingData.LoadedSharedLibraries)
			Platform::FreeSharedLibrary(lib);
		s_ScriptingData.LoadedSharedLibraries.clear();
	}
}