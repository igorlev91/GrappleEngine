#pragma once

#include "Grapple/Renderer/FrameBuffer.h"
#include "Grapple/Renderer/RenderData.h"
#include "Grapple/Renderer/Viewport.h"

#include "GrapplePlatform/Event.h"

#include <glm/glm.hpp>

#include <string>
#include <string_view>

namespace Grapple
{
	class ViewportWindow
	{
	public:
		ViewportWindow(std::string_view name, bool useEditorCamera = false);
		virtual ~ViewportWindow() = default;
	public:
		virtual void OnAttach();

		virtual void OnRenderImGui();
		virtual void OnRenderViewport();
		virtual void OnEvent(Event& event) {}

		const RenderData& GetRenderData() const { return m_Viewport.FrameData; }

		Viewport& GetViewport() { return m_Viewport; }
		const Viewport& GetViewport() const { return m_Viewport; }

		const std::string& GetName() const { return m_Name; }

		inline const bool HasFocusChanged() const { return m_PreviousFocusState != m_IsFocused; }
		inline const bool IsFocused() const { return m_IsFocused; }

		inline void RequestFocus() { m_WindowFocusRequested = true; }

		void PrepareViewport();
		void SetViewProjection(const glm::mat4& projection) { m_Viewport.FrameData.Camera.Projection = projection; }
	protected:
		void BeginImGui();
		void RenderViewportBuffer(const Ref<FrameBuffer>& buffer, uint32_t attachmentIndex);
		void EndImGui();

		virtual void CreateFrameBuffer();
		virtual void OnClear();
		virtual void OnViewportChanged() {}
	public:
		bool ShowWindow;
	protected:
		std::string m_Name;
		Viewport m_Viewport;
		bool m_PreviousFocusState;
		bool m_IsFocused;
		bool m_IsHovered;

		bool m_WindowFocusRequested;

		bool m_IsVisible;

		glm::ivec2 m_RelativeMousePosition;
		glm::ivec2 m_ViewportOffset;
	};
}