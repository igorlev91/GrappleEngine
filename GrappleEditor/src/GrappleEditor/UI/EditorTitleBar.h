#pragma once

#include "GrappleCore/Core.h"

#include "GrappleEditor/UI/WindowsWindowControls.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace Grapple
{
	class EditorLayer;
	class EditorTitleBar
	{
	public:
		EditorTitleBar();

		void OnRenderImGui();
	private:
		void RenderTitleBar();
	private:
		Ref<WindowsWindowControls> m_WindowControls;
	};
}