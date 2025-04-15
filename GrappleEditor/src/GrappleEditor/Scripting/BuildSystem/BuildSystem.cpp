#include "BuildSystem.h"

#include "Grapple/Project/Project.h"
#include "GrapplePlatform/Platform.h"

#include <filesystem>

namespace Grapple
{
	static std::filesystem::path ModuleNameToProjectPath(const std::string& name)
	{
		return Project::GetActive()->Location / fmt::format("{0}/{0}.vcxproj", name);
	}
	void Grapple::BuildSystem::BuildModules()
	{
		std::wstring configuration
	#ifdef Grapple_DEBUG
			= L"Debug";
	#elif Grapple_RELEASE
			= L"Release";
	#elif Grapple_DIST
			= L"Dist";
	#endif

		Ref<Project> activeProject = Project::GetActive();
		for (const std::string& name : activeProject->ScriptingModules)
		{
			std::filesystem::path msBuildPath = L"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe";
			std::filesystem::path projectPath = ModuleNameToProjectPath(name);

			if (!std::filesystem::exists(projectPath))
			{
				Grapple_CORE_WARN("Project file '{0}' doesn't exist", projectPath.generic_string());
				continue;
			}

			ProcessCreationSettings settings;
			settings.Arguments = projectPath.wstring() + L"/p:configuration=" + configuration + L" /p:platform=x64";
			settings.WorkingDirectory = projectPath.parent_path();
			int32_t exitCode = Platform::CreateProcess(msBuildPath, settings);

			if (exitCode != 0)
				Grapple_CORE_ERROR("MSBuild failed with exit code {0}", exitCode);
		}
	}
}
