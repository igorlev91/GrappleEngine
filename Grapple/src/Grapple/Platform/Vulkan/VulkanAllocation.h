#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Grapple
{
	struct VulkanAllocation
	{
		VmaAllocation Handle = VK_NULL_HANDLE;
		VmaAllocationInfo Info{};
	};
}
