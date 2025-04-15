#include "EditorApplication.h"

#include "GrappleCore/Log.h"
#include "Grapple/Project/Project.h"

#include "GrapplePlatform/Platform.h"

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
