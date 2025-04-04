#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace Grapple
{
	class EditorLayer;
	class EditorTitleBar
	{
	public:
		void OnRenderImGui();
	private:
		void RenderTitleBar();
	};
}