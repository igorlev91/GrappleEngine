#pragma once

#include "Grapple/Renderer/FrameBuffer.h"
#include "Grapple/Renderer/RenderData.h"

#include <glm/glm.hpp>

#include <string>
#include <string_view>

namespace Grapple
{
	class ViewportWindow
	{
	public:
		ViewportWindow(std::string_view name, bool useEditorCamera = false);
	public:
		virtual void OnRenderImGui();
		virtual void OnRenderViewport();

		void SetViewProjection(const glm::mat4& projection) { m_RenderData.Camera.ProjectionMatrix = projection; }
	protected:
		void BeginImGui();
		void EndImGui();

		virtual void CreateFrameBuffer();
		virtual void OnClear();
		virtual void OnViewportChanged() {}
	protected:
		std::string m_Name;
		Ref<FrameBuffer> m_FrameBuffer;
		RenderData m_RenderData;
		bool m_IsFocused;
		bool m_IsHovered;
		glm::ivec2 m_RelativeMousePosition;
		glm::ivec2 m_ViewportOffset;
	};
}