#pragma once

#include "GrappleCore/Core.h"

#include "GrappleEditor/UI/WindowsWindowControls.h"

#include <glm/glm.hpp>

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
		bool RenderButton(const char* id, glm::ivec2 iconPosition);
	private:
		Ref<WindowsWindowControls> m_WindowControls;
	};
}