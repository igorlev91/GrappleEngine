#pragma once

#include "Grapple/Renderer/FrameBuffer.h"
#include "Grapple/Renderer/RenderData.h"

#include <string>
#include <string_view>

namespace Grapple
{
	class ViewportWindow
	{
	public:
		ViewportWindow(std::string_view name, bool useEditorCamera = false);
	public:
		void OnRenderViewport();
		void OnRenderImGui();
	private:
		std::string m_Name;
		Ref<FrameBuffer> m_FrameBuffer;
		RenderData m_RenderData;
	};
}