#include "EditorApplication.h"

#include "Grapple/Core/Log.h"

#include "Grapple/Project/Project.h"
#include "Grapple/Platform/Platform.h"

#include "GrappleEditor/EditorLayer.h"

#include <filesystem>

namespace Grapple
{
	EditorApplication::EditorApplication(CommandLineArguments arguments)
	{
		Application::GetInstance().GetWindow()->SetTitle("Grapple Editor");

		if (arguments.ArgumentsCount >= 2)
		{
			std::filesystem::path projectPath = arguments[1];
			Project::OpenProject(projectPath);
		}
		else
		{
			std::optional<std::filesystem::path> projectPath = Platform::ShowOpenFileDialog(L"Grapple Project (*.Grappleproj)\0*.Grappleproj\0");

			if (projectPath.has_value())
				Project::OpenProject(projectPath.value());
		}

		PushLayer(CreateRef<EditorLayer>());
	}

	EditorApplication::~EditorApplication()
	{
	}
}
