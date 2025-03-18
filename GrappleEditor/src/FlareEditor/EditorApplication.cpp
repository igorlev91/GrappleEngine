#include "EditorApplication.h"

namespace Grapple
{
	EditorApplication::EditorApplication()
	{
		PushLayer(CreateRef<EditorLayer>());
	}

	EditorApplication::~EditorApplication()
	{
	}
}
