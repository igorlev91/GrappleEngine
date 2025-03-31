#pragma once

#include "GrappleEditor/ViewportWindow.h"
#include "GrappleEditor/EditorCamera.h"

namespace Grapple
{
	class SceneViewportWindow : public ViewportWindow
	{
	public:
		SceneViewportWindow(EditorCamera& camera)
			: ViewportWindow("Scene Viewport", true), m_Camera(camera) {}

		virtual void OnRenderViewport() override;
		virtual void OnViewportResize() override;
	private:
		EditorCamera& m_Camera;
	};
}