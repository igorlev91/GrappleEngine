#pragma once

#include "Grapple/Scene/Scene.h"
#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/Material.h"

#include "GrappleECS/Entity/Entity.h"

#include "GrappleEditor/Guizmo.h"
#include "GrappleEditor/ViewportWindow.h"
#include "GrappleEditor/EditorCamera.h"
#include "GrappleEditor/EditorCameraController.h"

namespace Grapple
{
	class SceneViewportWindow : public ViewportWindow
	{
	public:
		enum class ViewportOverlay
		{
			Default,
			Normal,
			Depth,
		};

		SceneViewportWindow(EditorCamera& camera, std::string_view name = "Scene Viewport");

		virtual void OnAttach() override;

		virtual void OnRenderViewport() override;
		virtual void OnViewportChanged() override;
		virtual void OnRenderImGui() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnAddRenderPasses() override;
	protected:
		virtual void CreateFrameBuffer() override;
		virtual void OnClear() override;
	private:
		void RenderWindowContents();
		void RenderToolBar();
		void RenderGrid();

		void HandleAssetDragAndDrop(AssetHandle handle);
		std::optional<Entity> GetEntityUnderCursor() const;
	private:
		GuizmoMode m_Guizmo;
		EditorCamera& m_Camera;
		EditorCameraController m_CameraController;
		bool m_IsToolbarHovered;

		ViewportOverlay m_Overlay;
		Ref<Material> m_SelectionOutlineMaterial;
		Ref<Material> m_GridMaterial;
	};
}