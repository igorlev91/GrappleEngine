#include "VulkanShader.h"

#include "Grapple/AssetManager/AssetManager.h"
#include "Grapple/Renderer/ShaderCacheManager.h"
#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer2D/Renderer2D.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

namespace Grapple
{
	VkFormat ShaderDataTypeToVulkanFormat(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Int:
			return VK_FORMAT_R32_SINT;
		case ShaderDataType::Int2:
			return VK_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:
			return VK_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:
			return VK_FORMAT_R32G32B32A32_SINT;

		case ShaderDataType::Float:
			return VK_FORMAT_R32_SFLOAT;
		case ShaderDataType::Float2:
			return VK_FORMAT_R32G32_SFLOAT;
		case ShaderDataType::Float3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case ShaderDataType::Float4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;

		case ShaderDataType::Sampler:
		case ShaderDataType::SamplerArray:
		case ShaderDataType::Matrix4x4:
			Grapple_CORE_ASSERT(false);
			return VK_FORMAT_UNDEFINED;
		}

		Grapple_CORE_ASSERT(false);
		return VK_FORMAT_UNDEFINED;
	}

	VulkanShader::~VulkanShader()
	{
		Grapple_CORE_ASSERT(VulkanContext::GetInstance().IsValid());
		for (auto& stageModule : m_Modules)
		{
			vkDestroyShaderModule(VulkanContext::GetInstance().GetDevice(), stageModule.Module, nullptr);
		}

		vkDestroyPipelineLayout(VulkanContext::GetInstance().GetDevice(), m_PipelineLayout, nullptr);
	}

	VkShaderModule VulkanShader::GetModuleForStage(ShaderStageType stage) const
	{
		for (const auto& stageModule : m_Modules)
		{
			if (stageModule.Stage == stage)
				return stageModule.Module;
		}

		return VK_NULL_HANDLE;
	}

	void VulkanShader::Load()
	{
		Grapple_CORE_ASSERT(AssetManager::IsAssetHandleValid(Handle));

		m_Valid = false;

		m_Modules.resize(2);
		m_Modules[0].Stage = ShaderStageType::Vertex;
		m_Modules[1].Stage = ShaderStageType::Pixel;

		bool hasValidCache = true;
		for (const ShaderStageModule& stageModule : m_Modules)
		{
			if (!ShaderCacheManager::GetInstance()->HasCache(Handle, ShaderTargetEnvironment::Vulkan, stageModule.Stage))
			{
				hasValidCache = false;
				break;
			}
		}

		if (!hasValidCache)
			return;

		m_NameToIndex.clear();
		m_Valid = true;

		m_Metadata = ShaderCacheManager::GetInstance()->FindShaderMetadata(Handle);
		m_Valid = m_Metadata != nullptr;

		if (!m_Valid)
			return;

		for (ShaderStageModule& stageModule : m_Modules)
		{
			auto cachedShader = ShaderCacheManager::GetInstance()->FindCache(
				Handle, ShaderTargetEnvironment::Vulkan,
				stageModule.Stage);

			if (!cachedShader.has_value())
			{
				Grapple_CORE_ERROR("Failed to find cached Vulkan shader code");

				m_Valid = false;
				break;
			}

			VkShaderModuleCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			info.codeSize = cachedShader->size() * sizeof(decltype(cachedShader)::value_type::value_type);
			info.pCode = cachedShader->data();

			VK_CHECK_RESULT(vkCreateShaderModule(VulkanContext::GetInstance().GetDevice(), &info, nullptr, &stageModule.Module));
		}

		// Generate material descriptor set layout
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;

			for (const auto& property : m_Metadata->Properties)
			{
				if (property.Type != ShaderDataType::Sampler && property.Type != ShaderDataType::SamplerArray)
					continue;

				VkDescriptorSetLayoutBinding& binding = bindings.emplace_back();
				binding.binding = property.Binding;
				binding.descriptorCount = (uint32_t)(property.Size / ShaderDataTypeSize(ShaderDataType::Sampler));
				binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			}

			if (bindings.size() > 0)
			{
				m_SetPool = CreateRef<VulkanDescriptorSetPool>(200, Span<VkDescriptorSetLayoutBinding>::FromVector(bindings));
			}
		}

		Ref<VulkanDescriptorSetLayout> primaryDescriptorSet = Renderer::GetPrimaryDescriptorSetLayout();
		Ref<VulkanDescriptorSetLayout> secondaryDescriptorSet = Renderer2D::GetDescriptorSetLayout();

		std::vector<VkPushConstantRange> pushConstantsRanges;
		VkDescriptorSetLayout descriptorSetLayouts[3] = { nullptr };

		descriptorSetLayouts[0] = primaryDescriptorSet->GetHandle();
		descriptorSetLayouts[1] = secondaryDescriptorSet->GetHandle();

		if (m_SetPool)
		{
			descriptorSetLayouts[2] = m_SetPool->GetLayout()->GetHandle();
		}

		for (size_t i = 0; i < m_Metadata->PushConstantsRanges.size(); i++)
		{
			if (m_Metadata->PushConstantsRanges[i].Size == 0)
				continue;

			VkPushConstantRange& range = pushConstantsRanges.emplace_back();
			switch (m_Metadata->PushConstantsRanges[i].Stage)
			{
			case ShaderStageType::Vertex:
				range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case ShaderStageType::Pixel:
				range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			}

			range.offset = (uint32_t)m_Metadata->PushConstantsRanges[i].Offset;
			range.size = (uint32_t)m_Metadata->PushConstantsRanges[i].Size;
		}

		// Create pipeline layout
		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = m_SetPool ? 3 : 2;
		layoutInfo.pSetLayouts = descriptorSetLayouts;
		layoutInfo.pushConstantRangeCount = (uint32_t)pushConstantsRanges.size();
		layoutInfo.pPushConstantRanges = pushConstantsRanges.data();

		VK_CHECK_RESULT(vkCreatePipelineLayout(VulkanContext::GetInstance().GetDevice(), &layoutInfo, nullptr, &m_PipelineLayout));

		if (!m_Valid)
			return;

		const auto& properties = m_Metadata->Properties;
		for (size_t i = 0; i < properties.size(); i++)
			m_NameToIndex.emplace(properties[i].Name, (uint32_t)i);
	}

	bool VulkanShader::IsLoaded() const
	{
		return m_Valid;
	}

	void VulkanShader::Bind()
	{
	}

	Ref<const ShaderMetadata> VulkanShader::GetMetadata() const
	{
		return m_Metadata;
	}

	const ShaderProperties& VulkanShader::GetProperties() const
	{
		return m_Metadata->Properties;
	}

	const ShaderOutputs& VulkanShader::GetOutputs() const
	{
		return m_Metadata->Outputs;
	}

	ShaderFeatures VulkanShader::GetFeatures() const
	{
		return m_Metadata->Features;
	}

	std::optional<uint32_t> VulkanShader::GetPropertyIndex(std::string_view name) const
	{
		auto it = m_NameToIndex.find(name);
		if (it == m_NameToIndex.end())
			return {};
		return it->second;
	}

	Ref<VulkanDescriptorSetLayout> VulkanShader::GetDescriptorSetLayout() const
	{
		return m_SetPool->GetLayout();
	}
}
