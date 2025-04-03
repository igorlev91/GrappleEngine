#include "Project.h"

#include "Grapple/Core/Assert.h"
#include "Grapple/Project/ProjectSerializer.h"
#include "Grapple/Scripting/ScriptingEngine.h"

namespace Grapple
{
	Ref<Project> Project::s_Active;
	std::filesystem::path Project::s_ProjectFileExtension = ".Grappleproj";

	void Project::New(std::string_view name, const std::filesystem::path& path)
	{
		Grapple_CORE_ASSERT(std::filesystem::is_directory(path));

		std::filesystem::create_directories(path);

		Ref<Project> newProject = CreateRef<Project>(path);
		newProject->Name = name;
		newProject->StartScene = NULL_ASSET_HANDLE;

		ProjectSerializer::Serialize(newProject, newProject->GetProjectFilePath());
		s_Active = newProject;
	}

	void Project::OpenProject(const std::filesystem::path& path)
	{
		Grapple_CORE_ASSERT(!std::filesystem::is_directory(path));
		Grapple_CORE_ASSERT(path.extension() == s_ProjectFileExtension);

		ScriptingEngine::UnloadAllModules();

		Ref<Project> project = CreateRef<Project>(path.parent_path());
		ProjectSerializer::Deserialize(project, path);

		s_Active = project;

		ScriptingEngine::LoadModules();
	}

	void Project::Save()
	{
		ProjectSerializer::Serialize(s_Active, s_Active->GetProjectFilePath());
	}
}
