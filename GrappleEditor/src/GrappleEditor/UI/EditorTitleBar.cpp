#include "EditorTitleBar.h"

#include "Grapple/Core/Application.h"
#include "Grapple/Core/Window.h"
#include "Grapple/Platform/Platform.h"
#include "Grapple/Project/Project.h"

#include "GrappleEditor/EditorContext.h"
#include "GrappleEditor/EditorLayer.h"
#include "GrappleEditor/UI/EditorGUI.h"

#include <imgui_internal.h>

namespace Grapple
{
	void EditorTitleBar::OnRenderImGui()
	{
		Ref<Window> window = Application::GetInstance().GetWindow();

		if (window->GetProperties().CustomTitleBar)
		{
			WindowControls& controls = window->GetWindowControls();

			if (controls.BeginTitleBar())
			{
				RenderTitleBar();

				controls.RenderControls();
				controls.EndTitleBar();
			}
		}
	}

	void EditorTitleBar::RenderTitleBar()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		auto prevLayout = window->DC.LayoutType;

		window->DC.LayoutType = ImGuiLayoutType_Horizontal;
		ImGui::SetCursorPos(ImVec2(8.0f, 8.0f));

		if (EditorGUI::BeginMenu("Project"))
		{
			ImGui::BeginDisabled(EditorContext::Instance.Mode != EditorMode::Edit);
			if (ImGui::MenuItem("Save"))
				Project::Save();

			if (ImGui::MenuItem("Open"))
			{
				std::optional<std::filesystem::path> projectPath = Platform::ShowOpenFileDialog(L"Grapple Project (*.Grappleproj)\0*.Grappleproj\0");

				if (projectPath.has_value())
					Project::OpenProject(projectPath.value());
			}
			ImGui::EndDisabled();

			EditorGUI::EndMenu();
		}

		if (EditorGUI::BeginMenu("Scene"))
		{
			ImGui::BeginDisabled(EditorContext::Instance.Mode != EditorMode::Edit);
			if (ImGui::MenuItem("Save"))
				EditorLayer::GetInstance().SaveActiveScene();
			if (ImGui::MenuItem("Save As"))
				EditorLayer::GetInstance().SaveActiveSceneAs();

			ImGui::EndDisabled();
			EditorGUI::EndMenu();
		}

		float buttonHeight = window->MenuBarHeight();

		ImVec2 buttonSize = ImVec2(60.0f, buttonHeight);
		if (EditorContext::Instance.Mode == EditorMode::Edit)
		{
			if (ImGui::Button("Play", buttonSize))
				EditorLayer::GetInstance().EnterPlayMode();
		}
		else
		{
			if (ImGui::Button("Stop", buttonSize))
				EditorLayer::GetInstance().ExitPlayMode();
		}

		window->DC.LayoutType = prevLayout;
	}
}
