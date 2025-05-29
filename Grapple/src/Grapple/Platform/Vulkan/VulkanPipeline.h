#pragma once

#include "Grapple/Renderer/Pipeline.h"
#include "Grapple/Platform/Vulkan/VulkanRenderPass.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecifications& specifications,
			const Ref<VulkanRenderPass>& renderPass,
			const Span<Ref<const VulkanDescriptorSetLayout>>& layouts);
		~VulkanPipeline();

		const PipelineSpecifications& GetSpecifications() const override;

		inline VkPipeline GetHandle() const { return m_Pipeline; }
		inline VkPipelineLayout GetLayoutHandle() const { return m_PipelineLayout; }
	private:
		PipelineSpecifications m_Specifications;

		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
	};
}
