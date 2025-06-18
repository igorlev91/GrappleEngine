#pragma once

#include "Grapple.h"
#include "Grapple/Core/Layer.h"
#include "Grapple/Core/CommandLineArguments.h"

#include "GrappleECS/ECSContext.h"

#include "GrapplePlatform/FileWatcher.h"

#include "GrappleEditor/UI/AssetEditor.h"

#include "GrappleEditor/UI/SceneWindow.h"
#include "GrappleEditor/UI/PropertiesWindow.h"
#include "GrappleEditor/UI/AssetManagerWindow.h"

#include "GrappleEditor/UI/EditorTitleBar.h"
#include "GrappleEditor/UI/PrefabEditor.h"
#include "GrappleEditor/UI/SpriteEditor.h"
#include "GrappleEditor/UI/QuickSearch/QuickSearch.h"
#include "GrappleEditor/UI/PostProcessingWindow.h"

#include "GrappleEditor/ViewportWindow.h"
#include "GrappleEditor/EditorCamera.h"

#include "GrappleEditor/EditorSelection.h"
#include "GrappleEditor/SceneViewSettings.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"

#include <vector>
#include <set>

namespace Grapple
{
	enum class EditorMode
	{
		Edit,
		Play,
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer();
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;

		virtual void OnEvent(Event& event) override;
		virtual void OnImGUIRender() override;

		void SaveActiveScene();
		void SaveActiveSceneAs();

		void OpenScene(AssetHandle handle);
		void CreateNewScene();

		void EnterPlayMode();
		void ExitPlayMode();

		void ReloadScriptingModules();

		inline bool IsPlaymodePaused() const { return m_PlaymodePaused; }
		inline void SetPlaymodePaused(bool value) { m_PlaymodePaused = value; }

		inline EditorMode GetMode() const { return m_Mode; }

		inline EditorCamera& GetCamera() { return m_Camera; }
		inline const EditorCamera& GetCamera() const { return m_Camera; }

		inline ECSContext& GetECSContext() { return m_ECSContext; }
		inline const ECSContext& GetECSContext() const { return m_ECSContext; }

		inline const std::vector<Ref<ViewportWindow>>& GetViewportWindows() const { return m_ViewportWindows; }
		void SetFullscreenViewportWindow(Ref<ViewportWindow> viewportWindow);

		inline SceneViewSettings& GetSceneViewSettings() { return m_SceneViewSettings; }
		inline const SceneViewSettings& GetSceneViewSettings() const { return m_SceneViewSettings; }

		static EditorLayer& GetInstance();
	private:
		void UpdateWindowTitle();
		void OnOpenProject();

		void ResetViewportRenderGraphs();
	private:
		bool m_UpdateCursorModeNextFrame = false;
		bool m_EnterPlayModeScheduled = false;
		bool m_ExitPlayModeScheduled = false;

		std::set<AssetHandle> m_AssetReloadQueue;

		SceneViewSettings m_SceneViewSettings;

		Ref<ImGuiLayer> m_ImGuiLayer = nullptr;

		EditorTitleBar m_TitleBar;
		Ref<ViewportWindow> m_GameWindow;

		Ref<ViewportWindow> m_FullscreenViewport = nullptr;

		std::vector<Ref<AssetEditor>> m_AssetEditorWindows;
		Ref<PrefabEditor> m_PrefabEditor;
		Ref<SpriteEditor> m_SpriteEditor;

		SceneWindow m_SceneWindow;
		PropertiesWindow m_PropertiesWindow;
		AssetManagerWindow m_AssetManagerWindow;
		PostProcessingWindow m_PostProcessingWindow;
		QuickSearch m_QuickSearch;

		std::vector<Ref<ViewportWindow>> m_ViewportWindows;

		EditorCamera m_Camera;
		AssetHandle m_EditedSceneHandle;
		bool m_PlaymodePaused;
		EditorMode m_Mode;

		ECSContext m_ECSContext;

		Scope<FileWatcher> m_ProjectFilesWacher;
	public:
		EditorSelection Selection;
	private:
		static EditorLayer* s_Instance;
	};
}