#include "EditorTitleBar.h"

#include "Grapple/Core/Application.h"
#include "Grapple/Core/Window.h"

#include <imgui_internal.h>

namespace Grapple
{
	void EditorTitleBar::OnRenderImGui()
	{
		Ref<Window> window = Application::GetInstance().GetWindow();

		if (window->GetProperties().CustomTitleBar)
		{
			WindowControls& controls = window->GetWindowControls();

			controls.BeginTitleBar();

			RenderTitleBar();

			controls.RenderControls();
			controls.EndTitleBar();
		}
	}

	void EditorTitleBar::RenderTitleBar()
	{
		ImGui::SameLine();
	}
}
