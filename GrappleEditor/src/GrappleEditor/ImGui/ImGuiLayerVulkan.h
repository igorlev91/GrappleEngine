#pragma once

#include "GrappleEditor/ImGui/ImGuiLayer.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	class ImGuiLayerVulkan : public ImGuiLayer
	{
	public:
		void InitializeRenderer() override;
		void ShutdownRenderer() override;
		void InitializeFonts() override;

		void Begin() override;
		void End() override;

		void RenderCurrentWindow() override;
		void UpdateWindows() override;
	private:
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
	};
}
