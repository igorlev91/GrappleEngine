#include "EditorLayer.h"

#include "Grapple.h"
#include "Grapple/Core/Application.h"
#include "Grapple/Renderer2D/Renderer2D.h"
#include "Grapple/Scene/SceneSerializer.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Project/Project.h"
#include "Grapple/Platform/Platform.h"

#include "Grapple/Scripting/ScriptingEngine.h"
#include "Grapple/Input/InputManager.h"

#include "GrappleEditor/AssetManager/EditorAssetManager.h"

#include "GrappleEditor/UI/SceneViewportWindow.h"
#include "GrappleEditor/UI/EditorTitleBar.h"
#include "GrappleEditor/UI/SystemsInspectorWindow.h"
#include "GrappleEditor/UI/ProjectSettingsWindow.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Grapple
{
	EditorLayer* EditorLayer::s_Instance = nullptr;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), 
		m_EditedSceneHandle(NULL_ASSET_HANDLE), 
		m_PlaymodePaused(false),
		m_Mode(EditorMode::Edit),
		m_Guizmo(GuizmoMode::None)
	{
		s_Instance = this;
	}

	EditorLayer::~EditorLayer()
	{
		s_Instance = nullptr;
	}

	void EditorLayer::OnAttach()
	{
		Project::OnProjectOpen.Bind(Grapple_BIND_EVENT_CALLBACK(OnOpenProject));
		Project::OnUnloadActiveProject.Bind([this]()
		{
			Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());

			assetManager->UnloadAsset(Scene::GetActive()->Handle);
			Scene::SetActive(nullptr);
		});

		AssetManager::Intialize(CreateRef<EditorAssetManager>());

		m_AssetManagerWindow.SetOpenAction(AssetType::Scene, [this](AssetHandle handle)
		{
			OpenScene(handle);
		});

		m_Viewports.emplace_back(CreateRef<SceneViewportWindow>(m_Camera));
		m_Viewports.emplace_back(CreateRef<ViewportWindow>("Game"));

		EditorCameraSettings& settings = m_Camera.GetSettings();
		settings.FOV = 60.0f;
		settings.Near = 0.1f;
		settings.Far = 1000.0f;
		settings.RotationSpeed = 1.0f;
		settings.DragSpeed = 0.1f;

		if (Application::GetInstance().GetCommandLineArguments().ArgumentsCount >= 2)
		{
			std::filesystem::path projectPath = Application::GetInstance().GetCommandLineArguments()[1];
			Project::OpenProject(projectPath);
		}
		else
		{
			std::optional<std::filesystem::path> projectPath = Platform::ShowOpenFileDialog(L"Grapple Project (*.Grappleproj)\0*.Grappleproj\0");

			if (projectPath.has_value())
				Project::OpenProject(projectPath.value());
		}
	}

	void EditorLayer::OnDetach()
	{
		if (m_Mode == EditorMode::Play)
			ExitPlayMode();

		if (AssetManager::IsAssetHandleValid(Scene::GetActive()->Handle))
			As<EditorAssetManager>(AssetManager::GetInstance())->UnloadAsset(Scene::GetActive()->Handle);

		Scene::SetActive(nullptr);
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		m_PreviousFrameTime = deltaTime;

		Renderer2D::ResetStats();

		ScriptingEngine::OnFrameStart(deltaTime);

		if (m_Mode == EditorMode::Play && !m_PlaymodePaused)
			Scene::GetActive()->OnUpdateRuntime();

		for (auto& viewport : m_Viewports)
			viewport->OnRenderViewport();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_Camera.ProcessEvents(event);
		InputManager::ProcessEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& e) -> bool
		{
			switch (e.GetKeyCode())
			{
			case KeyCode::G:
				m_Guizmo = GuizmoMode::Translate;
				break;
			case KeyCode::R:
				m_Guizmo = GuizmoMode::Rotate;
				break;
			case KeyCode::S:
				m_Guizmo = GuizmoMode::Scale;
				break;
			}

			return false;
		});
	}

	void EditorLayer::OnImGUIRender()
	{
		static bool fullscreen = true;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
		if (fullscreen)
		{
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

		EditorTitleBar titleBar;
		titleBar.OnRenderImGui();

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
			viewport->OnRenderImGui();

		SystemsInspectorWindow::OnImGuiRender();
		ProjectSettingsWindow::OnRenderImGui();

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

	void EditorLayer::OnOpenProject()
	{
		Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());

		assetManager->Reinitialize();

		UpdateWindowTitle();
		m_AssetManagerWindow.RebuildAssetTree();

		AssetHandle startScene = Project::GetActive()->StartScene;
		OpenScene(startScene);
	}

	void EditorLayer::SaveActiveScene()
	{
		Grapple_CORE_ASSERT(Scene::GetActive());
		if (AssetManager::IsAssetHandleValid(Scene::GetActive()->Handle))
			SceneSerializer::Serialize(Scene::GetActive());
		else
			SaveActiveSceneAs();
	}

	void EditorLayer::SaveActiveSceneAs()
	{
		std::optional<std::filesystem::path> scenePath = Platform::ShowSaveFileDialog(L"Grapple Scene (*.Grapple)\0*.Grapple\0");
		if (scenePath.has_value())
		{
			std::filesystem::path& path = scenePath.value();
			if (!path.has_extension())
				path.replace_extension(".Grapple");

			SceneSerializer::Serialize(Scene::GetActive(), path);
			AssetHandle handle = As<EditorAssetManager>(AssetManager::GetInstance())->ImportAsset(path);
			OpenScene(handle);
		}
	}

	void EditorLayer::OpenScene(AssetHandle handle)
	{
		if (AssetManager::IsAssetHandleValid(handle))
		{
			Ref<Scene> active = Scene::GetActive();

			Ref<EditorAssetManager> editorAssetManager = As<EditorAssetManager>(AssetManager::GetInstance());

			if (active != nullptr && AssetManager::IsAssetHandleValid(active->Handle))
				editorAssetManager->UnloadAsset(active->Handle);

			active = nullptr;
			Scene::SetActive(nullptr);
			ScriptingEngine::UnloadAllModules();

			ScriptingEngine::LoadModules();

			active = AssetManager::GetAsset<Scene>(handle);
			Scene::SetActive(active);

			active->InitializeRuntime();

			m_EditedSceneHandle = handle;
		}
	}

	void EditorLayer::EnterPlayMode()
	{
		Grapple_CORE_ASSERT(m_Mode == EditorMode::Edit);

		Ref<Scene> active = Scene::GetActive();

		Ref<EditorAssetManager> assetManager = As<EditorAssetManager>(AssetManager::GetInstance());
		std::filesystem::path activeScenePath = assetManager->GetAssetMetadata(active->Handle)->Path;
		SaveActiveScene();

		m_PlaymodePaused = false;

		Scene::SetActive(nullptr);
		assetManager->UnloadAsset(active->Handle);
		active = nullptr;

		ScriptingEngine::UnloadAllModules();

		ScriptingEngine::LoadModules();
		Ref<Scene> playModeScene = CreateRef<Scene>();
		SceneSerializer::Deserialize(playModeScene, activeScenePath);

		Scene::SetActive(playModeScene);
		m_Mode = EditorMode::Play;

		playModeScene->InitializeRuntime();
		Scene::GetActive()->OnRuntimeStart();
	}

	void EditorLayer::ExitPlayMode()
	{
		Grapple_CORE_ASSERT(m_Mode == EditorMode::Play);

		Scene::GetActive()->OnRuntimeEnd();

		Scene::SetActive(nullptr);
		ScriptingEngine::UnloadAllModules();

		ScriptingEngine::LoadModules();
	 	Ref<Scene> editorScene = AssetManager::GetAsset<Scene>(m_EditedSceneHandle);
		editorScene->InitializeRuntime();

		Scene::SetActive(editorScene);
		m_Mode = EditorMode::Edit;
	}

	EditorLayer& EditorLayer::GetInstance()
	{
		Grapple_CORE_ASSERT(s_Instance != nullptr, "Invalid EditorLayer instance");
		return *s_Instance;
	}
}