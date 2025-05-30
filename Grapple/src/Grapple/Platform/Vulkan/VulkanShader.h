#pragma once

#include "Grapple/Renderer/Shader.h"

#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

namespace Grapple
{
	class VulkanDescriptorSetLayout;
	class VulkanDescriptorSetPool;

	Grapple_API VkFormat ShaderDataTypeToVulkanFormat(ShaderDataType type);

	class VulkanShader : public Shader
	{
	public:
		~VulkanShader();

		VkShaderModule GetModuleForStage(ShaderStageType stage) const;

		void Load() override;
		bool IsLoaded() const override;
		void Bind() override;

		Ref<const ShaderMetadata> GetMetadata() const override;
		const ShaderProperties& GetProperties() const override;
		const ShaderOutputs& GetOutputs() const override;
		ShaderFeatures GetFeatures() const override;
		std::optional<uint32_t> GetPropertyIndex(std::string_view name) const override;

		inline VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

		Ref<VulkanDescriptorSetLayout> GetDescriptorSetLayout() const;
		Ref<VulkanDescriptorSetPool> GetDescriptorSetPool() const { return m_SetPool; }
	private:
		struct ShaderStageModule
		{
			ShaderStageType Stage = ShaderStageType::Vertex;
			VkShaderModule Module = VK_NULL_HANDLE;
		};

		bool m_Valid = false;

		Ref<const ShaderMetadata> m_Metadata = nullptr;
		std::vector<ShaderStageModule> m_Modules;

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		Ref<VulkanDescriptorSetPool> m_SetPool = nullptr;

		std::unordered_map<std::string_view, uint32_t> m_NameToIndex;
	};
}
