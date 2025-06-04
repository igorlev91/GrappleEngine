#include "VulkanMaterial.h"

#include "Grapple/AssetManager/AssetManager.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Platform/Vulkan/VulkanPipeline.h"
#include "Grapple/Platform/Vulkan/VulkanShader.h"
#include "Grapple/Platform/Vulkan/VulkanDescriptorSet.h"

#include "Grapple/Renderer/Renderer.h"
#include "Grapple/Renderer2D/Renderer2D.h"

namespace Grapple
{
	void VulkanMaterial::SetShader(const Ref<Shader>& shader)
	{
		if (m_Shader && m_Set)
		{
			Ref<VulkanShader> vulkanShader = As<VulkanShader>(m_Shader);
			auto pool = vulkanShader->GetDescriptorSetPool();

			if (pool)
			{
				pool->ReleaseSet(m_Set);
			}
		}

		Material::SetShader(shader);

		if (shader == nullptr)
		{
			return;
		}

		m_Pipeline = nullptr;
		m_Set = nullptr;

		Ref<VulkanShader> vulkanShader = As<VulkanShader>(shader);
		auto pool = vulkanShader->GetDescriptorSetPool();

		if (pool)
		{
			m_Set = As<VulkanDescriptorSet>(pool->AllocateSet());

			const AssetMetadata* metadata = AssetManager::GetAssetMetadata(Handle);
			if (metadata != nullptr)
			{
				m_Set->SetDebugName(metadata->Name);
			}
			else
			{
				m_Set->SetDebugName(As<VulkanShader>(m_Shader)->GetDebugName());
			}
		}
	}

	Ref<VulkanPipeline> VulkanMaterial::GetPipeline(const Ref<VulkanRenderPass>& renderPass)
	{
		if (m_Pipeline != nullptr && m_Pipeline->GetCompatibleRenderPass().get() == renderPass.get())
			return m_Pipeline;

		m_Pipeline = As<VulkanPipeline>(VulkanContext::GetInstance().GetDefaultPipelineForShader(m_Shader, renderPass));
		return m_Pipeline;
	}

	void VulkanMaterial::UpdateDescriptorSet()
	{
		if (!m_IsDirty)
			return;

		const auto& properties = m_Shader->GetMetadata()->Properties;
		for (size_t i = 0; i < properties.size(); i++)
		{
			const auto& property = properties[i];
			if (property.Type != ShaderDataType::Sampler)
				continue;

			const auto& textureProperty = ReadPropertyValue<TexturePropertyValue>((uint32_t)i);
			if (textureProperty.Texture)
			{
				m_Set->WriteImage(textureProperty.Texture, property.Binding);
			}
			else if (textureProperty.FrameBuffer)
			{
				// NOTE: FrameBuffer attachment is expected to have the right layout
				m_Set->WriteImage(textureProperty.FrameBuffer, textureProperty.FrameBufferAttachmentIndex, property.Binding);
			}
			else
			{
				Grapple_CORE_WARN("Material has an invalid texture property at index {}. A white texture is used instead", i);
				m_Set->WriteImage(Renderer::GetWhiteTexture(), property.Binding);
			}
		}

		m_Set->FlushWrites();

		m_IsDirty = false;
	}
}
