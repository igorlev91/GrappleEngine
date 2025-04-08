#include "EditorApplication.h"

#include "Grapple/Core/Log.h"

#include "Grapple/Project/Project.h"
#include "Grapple/Platform/Platform.h"

#include "GrappleEditor/EditorLayer.h"

#include <filesystem>

namespace Grapple
{
	EditorApplication::EditorApplication(CommandLineArguments arguments)
		: Application(arguments)
	{
		Application::GetInstance().GetWindow()->SetTitle("Grapple Editor");
		PushLayer(CreateRef<EditorLayer>());
	}

	EditorApplication::~EditorApplication()
	{
	}
}
