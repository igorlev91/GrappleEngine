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
		VulkanComputeShader();
		~VulkanComputeShader();

		Ref<const ComputeShaderMetadata> GetMetadata() const override;
		void Load() override;
		bool IsLoaded() const override;

		inline VkShaderModule GetModule() const { return m_Module; }
		inline VkPipelineLayout GetPipelineLayoutHandle() const { return m_PipelineLayout; }
		inline Ref<VulkanDescriptorSetPool> GetSetPool() const { return m_SetPool; }
	private:
		void CreatePipelineLayout();
	private:
		Ref<const ComputeShaderMetadata> m_Metadata = nullptr;
		bool m_IsLoaded = false;

		VkShaderModule m_Module = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		Ref<VulkanDescriptorSetPool> m_SetPool = nullptr;
	};
}
