#include "EditorApplication.h"

namespace Grapple
{
	EditorApplication::EditorApplication()
	{
		Renderer2D::Initialize();

		PushLayer(CreateRef<EditorLayer>());
	}

	EditorApplication::~EditorApplication()
	{
		Renderer2D::Shutdown();
	}
}
