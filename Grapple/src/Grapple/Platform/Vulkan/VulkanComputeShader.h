#pragma once

#include "GrappleCore/Collections/Span.h"
#include "Grapple/Renderer/ComputeShader.h"

#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	class VulkanComputeShader : public ComputeShader
	{
	public:
		VulkanComputeShader(Span<Ref<VulkanDescriptorSetLayout>> layouts);
		~VulkanComputeShader();

		Ref<const ComputeShaderMetadata> GetMetadata() const override;
		void Load() override;
		bool IsLoaded() const override;

		inline VkShaderModule GetModuleHandle() const { return m_Module; }
	private:
		void CreatePipelineLayout();
	private:
		Ref<const ComputeShaderMetadata> m_Metadata = nullptr;
		std::vector<Ref<VulkanDescriptorSetLayout>> m_DescriptorSetLayouts;
		bool m_IsLoaded = false;

		VkShaderModule m_Module = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
	};
}
