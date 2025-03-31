#include "EditorLayer.h"

#include "Grapple.h"
#include "Grapple/Core/Application.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Scene/SceneSerializer.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Project/Project.h"
#include "Grapple/Platform/Platform.h"

#include "GrappleEditor/EditorContext.h"
#include "GrappleEditor/AssetManager/EditorAssetManager.h"

#include <imgui.h>

namespace Grapple
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		UpdateWindowTitle();

		AssetManager::Intialize(CreateRef<EditorAssetManager>(Project::GetActive()->Location / "Assets"));
		m_AssetManagerWindow.RebuildAssetTree();

		m_AssetManagerWindow.SetOpenAction(AssetType::Scene, [this](AssetHandle handle)
		{
			EditorContext::OpenScene(handle);
		});

		EditorContext::Initialize();

		m_Viewports.emplace_back("Scene Viewport");
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		m_PreviousFrameTime = deltaTime;

		Renderer2D::ResetStats();

		EditorContext::GetActiveScene()->OnUpdateRuntime();

		for (auto& viewport : m_Viewports)
			viewport.OnRenderViewport();
	}

	void EditorLayer::OnEvent(Event& event)
	{
	}

	void EditorLayer::OnImGUIRender()
	{
		static bool fullscreen = true;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		static bool open = true;
		ImGui::Begin("DockSpace", &open, windowFlags);

		ImGuiID dockspaceId = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Save"))
				{
					Project::Save();
				}

				if (ImGui::MenuItem("Open"))
				{
					std::optional<std::filesystem::path> projectPath = Platform::ShowOpenFileDialog(L"Grapple Project (*.Grappleproj)\0*.Grappleproj\0");

					if (projectPath.has_value())
						Project::OpenProject(projectPath.value());
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("Save"))
				{
					if (AssetManager::IsAssetHandleValid(EditorContext::GetActiveScene()->Handle))
						SceneSerializer::Serialize(EditorContext::GetActiveScene());
					// else
					// TODO: Ask for save location
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		{
			ImGui::Begin("Renderer");

			ImGui::Text("Frame time %f", m_PreviousFrameTime);
			ImGui::Text("FPS %f", 1.0f / m_PreviousFrameTime);

			if (ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_ClearColor), ImGuiColorEditFlags_Float))
				RenderCommand::SetClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0);

			if (ImGui::CollapsingHeader("Renderer 2D"))
			{
				const auto& stats = Renderer2D::GetStats();
				ImGui::Text("Quads %d", stats.QuadsCount);
				ImGui::Text("Draw Calls %d", stats.DrawCalls);
				ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
			}

			ImGui::End();
		}

		{
			ImGui::Begin("Settings");

			Ref<Window> window = Application::GetInstance().GetWindow();

			bool vsync = window->GetProperties().VSyncEnabled;
			if (ImGui::Checkbox("VSync", &vsync))
				window->SetVSync(vsync);

			ImGui::End();
		}

		for (auto& viewport : m_Viewports)
			viewport.OnRenderImGui();

		m_SceneWindow.OnImGuiRender();
		m_PropertiesWindow.OnImGuiRender();
		m_AssetManagerWindow.OnImGuiRender();

		ImGui::End();
	}

	void EditorLayer::UpdateWindowTitle()
	{
		if (Project::GetActive() != nullptr)
		{
			std::string name = fmt::format("Grapple Editor - {0} - {1}", Project::GetActive()->Name, Project::GetActive()->Location.generic_string());
			Application::GetInstance().GetWindow()->SetTitle(name);
		}
	}
}