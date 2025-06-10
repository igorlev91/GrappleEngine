#pragma once

#include "Grapple/Renderer/Sampler.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	class VulkanSampler : public Sampler
	{
	public:
		VulkanSampler(const SamplerSpecifications& specifications);
		~VulkanSampler();

		const SamplerSpecifications& GetSpecifications() const override;

		VkSampler GetHandle() const { return m_Sampler; }
	private:
		SamplerSpecifications m_Specifications;
		VkSampler m_Sampler = VK_NULL_HANDLE;
	};
}
