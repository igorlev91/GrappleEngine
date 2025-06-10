#pragma once

#include "GrappleCore/Core.h"

#include "GrappleEditor/ImGui/ImGuiLayer.h"

namespace Grapple
{
	class Scene;
	class PostProcessingWindow
	{
	public:
		PostProcessingWindow() = default;
		PostProcessingWindow(Ref<Scene> scene);

		void OnImGuiRender();
	private:
		void RenderWindowContents();
		bool RenderCheckBoxAndLabel(void* id, ImVec2 position, ImVec2 treeNodeSize, bool* enabled, const char* label);
	private:
		Ref<Scene> m_Scene = nullptr;
	};
}
