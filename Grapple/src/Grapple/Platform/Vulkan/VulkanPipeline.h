#pragma once

#include "Grapple/Renderer/Pipeline.h"
#include "Grapple/Platform/Vulkan/VulkanRenderPass.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	class Grapple_API VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecifications& specifications,
			const Ref<VulkanRenderPass>& renderPass,
			const Span<Ref<const DescriptorSetLayout>>& layouts,
			const Span<ShaderPushConstantsRange>& pushConstantsRanges);
		VulkanPipeline(const PipelineSpecifications& specifications, const Ref<VulkanRenderPass>& renderPass);
		~VulkanPipeline();

		const PipelineSpecifications& GetSpecifications() const override;

		inline VkPipeline GetHandle() const { return m_Pipeline; }
		inline VkPipelineLayout GetLayoutHandle() const { return m_PipelineLayout; }
		inline Ref<VulkanRenderPass> GetCompatibleRenderPass() const { return m_CompatbileRenderPass; }
	private:
		void CreatePipelineLayout(const Span<Ref<const DescriptorSetLayout>>& layouts, const Span<ShaderPushConstantsRange>& pushConstantsRanges);
		void Create();
	private:
		PipelineSpecifications m_Specifications;
		Ref<VulkanRenderPass> m_CompatbileRenderPass = nullptr;

		bool m_OwnsPipelineLayout = true;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
	};
}
