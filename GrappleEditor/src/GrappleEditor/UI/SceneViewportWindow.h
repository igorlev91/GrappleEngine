#pragma once

#include "Grapple/Renderer/Shader.h"
#include "Grapple/Renderer/VertexArray.h"

#include "GrappleECS/Entity/Entity.h"

#include "GrappleEditor/ViewportWindow.h"
#include "GrappleEditor/EditorCamera.h"

namespace Grapple
{
	class SceneViewportWindow : public ViewportWindow
	{
	public:
		SceneViewportWindow(EditorCamera& camera);

		virtual void OnRenderViewport() override;
		virtual void OnViewportChanged() override;
		virtual void OnRenderImGui() override;
	protected:
		virtual void CreateFrameBuffer() override;
		virtual void OnClear() override;
	private:
		Entity GetEntityUnderCursor() const;
	private:
		EditorCamera& m_Camera;
		Ref<Shader> m_SelectionOutlineShader;
		Ref<VertexArray> m_FullscreenQuad;
		Ref<FrameBuffer> m_ScreenBuffer;
	};
}