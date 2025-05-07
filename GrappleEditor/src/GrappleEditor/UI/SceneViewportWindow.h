#pragma once

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/Material.h"
#include "Grapple/Renderer/VertexArray.h"

#include "GrappleECS/Entity/Entity.h"

#include "GrappleEditor/ViewportWindow.h"
#include "GrappleEditor/EditorCamera.h"

namespace Grapple
{
	class SceneViewportWindow : public ViewportWindow
	{
	public:
		enum class ViewportOverlay
		{
			Default,
			Depth,
		};

		SceneViewportWindow(EditorCamera& camera);

		virtual void OnAttach() override;

		virtual void OnRenderViewport() override;
		virtual void OnViewportChanged() override;
		virtual void OnRenderImGui() override;
		virtual void OnEvent(Event& event) override;
	protected:
		virtual void CreateFrameBuffer() override;
		virtual void OnClear() override;
	private:
		void RenderWindowContents();
		void RenderToolBar();
		void RenderGrid();

		Entity GetEntityUnderCursor() const;
	private:
		EditorCamera& m_Camera;
		bool m_IsToolbarHovered;

		ViewportOverlay m_Overlay;
		Ref<Material> m_SelectionOutlineMaterial;
		Ref<Material> m_GridMaterial;
		Ref<FrameBuffer> m_ScreenBuffer;
	};
}