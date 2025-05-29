#pragma once

#include "GrappleCore/Collections/Span.h"

#include "Grapple/Renderer/UniformBuffer.h"
#include "Grapple/Renderer/Texture.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Grapple
{
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout(const Span<VkDescriptorSetLayoutBinding>& bindings);
		~VulkanDescriptorSetLayout();

		inline VkDescriptorSetLayout GetHandle() const { return m_Layout; };
		inline uint32_t GetImageBindingsCount() const { return m_ImageBindings; }
		inline uint32_t GetBufferBindingsCount() const { return m_BufferBindings; }
	private:
		VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
		uint32_t m_ImageBindings = 0;
		uint32_t m_BufferBindings = 0;
	};

	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VkDescriptorPool pool, const Ref<const VulkanDescriptorSetLayout>& layout);
		~VulkanDescriptorSet();

		void WriteUniformBuffer(const Ref<const UniformBuffer>& uniformBuffer, uint32_t binding);
		void WriteTexture(const Ref<const Texture>& texture, uint32_t binding);
		void WriteTextures(const Span<Ref<const Texture>>& textures, size_t arrayOffset, uint32_t binding);

		void FlushWrites();

		inline VkDescriptorSet GetHandle() const { return m_Set; }
	private:
		VkDescriptorSet m_Set = VK_NULL_HANDLE;

		std::vector<VkWriteDescriptorSet> m_Writes;
		std::vector<VkDescriptorImageInfo> m_Images;
		std::vector<VkDescriptorBufferInfo> m_Buffers;
	};
}
