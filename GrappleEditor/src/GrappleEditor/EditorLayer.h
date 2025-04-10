#pragma once

#include "Grapple.h"
#include "Grapple/Core/Layer.h"
#include "Grapple/Core/CommandLineArguments.h"

#include "GrappleEditor/UI/SceneWindow.h"
#include "GrappleEditor/UI/PropertiesWindow.h"
#include "GrappleEditor/UI/AssetManagerWindow.h"

#include "GrappleEditor/UI//EditorTitleBar.h"

#include "GrappleEditor/ViewportWindow.h"
#include "GrappleEditor/EditorCamera.h"

#include <vector>

namespace Grapple
{
	enum class EditorMode
	{
		Edit,
		Play,
	};

	enum class GuizmoMode
	{
		None,
		Translate,
		Rotate,
		Scale,
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

		void EnterPlayMode();
		void ExitPlayMode();

		inline bool IsPlaymodePaused() const { return m_PlaymodePaused; }
		inline void SetPlaymodePaused(bool value) { m_PlaymodePaused = value; }

		inline Entity GetSelectedEntity() const { return m_SelectedEntity; }
		inline void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

		inline GuizmoMode GetGuizmoMode() const { return m_Guizmo; }
		inline EditorMode GetMode() const { return m_Mode; }

		static EditorLayer& GetInstance();
	private:
		void UpdateWindowTitle();
		void OnOpenProject();
	private:
		float m_PreviousFrameTime = 0.0f;

		EditorTitleBar m_TitleBar;

		SceneWindow m_SceneWindow;
		PropertiesWindow m_PropertiesWindow;
		AssetManagerWindow m_AssetManagerWindow;

		std::vector<Ref<ViewportWindow>> m_Viewports;
		glm::vec4 m_ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		EditorCamera m_Camera;
		AssetHandle m_EditedSceneHandle;
		bool m_PlaymodePaused;
		Entity m_SelectedEntity;
		EditorMode m_Mode;
		GuizmoMode m_Guizmo;

		static EditorLayer* s_Instance;
	};
}