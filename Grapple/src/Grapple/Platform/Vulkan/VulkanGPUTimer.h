#pragma once

#include "Grapple/Renderer/GPUTimer.h"

#include <vulkan/vulkan.h>

#include <optional>

namespace Grapple
{
	class VulkanGPUTimer : public GPUTimer
	{
	public:
		VulkanGPUTimer();
		~VulkanGPUTimer();

		std::optional<float> GetElapsedTime() override;

		VkQueryPool GetPoolHandle() const { return m_Pool; }
	private:
		VkQueryPool m_Pool = VK_NULL_HANDLE;
	};
}
