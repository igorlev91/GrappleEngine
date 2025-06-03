#pragma once

#include "GrappleCore/Collections/Span.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Grapple
{
	class UniformBuffer;
	class ShaderStorageBuffer;
	class Texture;
	class FrameBuffer;
	class VulkanDescriptorSetPool;

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
		VulkanDescriptorSet(VulkanDescriptorSetPool* pool, VkDescriptorSet set);
		~VulkanDescriptorSet();

		void WriteUniformBuffer(const Ref<const UniformBuffer>& uniformBuffer, uint32_t binding);
		void WriteStorageBuffer(const Ref<const ShaderStorageBuffer>& storageBuffer, uint32_t binding);
		void WriteTexture(const Ref<const Texture>& texture, uint32_t binding);
		void WriteFrameBufferAttachment(const Ref<const FrameBuffer>& frameBuffer, uint32_t attachmentIndex, uint32_t binding);
		void WriteStorageImage(const Ref<const FrameBuffer>& frameBuffer, uint32_t attachmentIndex, uint32_t binding);
		void WriteTextures(const Span<Ref<const Texture>>& textures, size_t arrayOffset, uint32_t binding);

		void FlushWrites();

		void SetDebugName(std::string_view name);
		const std::string& GetDebugName() const { return m_DebugName; }

		inline VkDescriptorSet GetHandle() const { return m_Set; }
	private:
		std::string m_DebugName;
		VkDescriptorSet m_Set = VK_NULL_HANDLE;
		VulkanDescriptorSetPool* m_Pool = VK_NULL_HANDLE;

		std::vector<VkWriteDescriptorSet> m_Writes;
		std::vector<VkDescriptorImageInfo> m_Images;
		std::vector<VkDescriptorBufferInfo> m_Buffers;
	};

	class VulkanDescriptorSetPool
	{
	public:
		VulkanDescriptorSetPool(size_t maxSets, const Span<VkDescriptorSetLayoutBinding>& bindings);
		~VulkanDescriptorSetPool();

		Ref<VulkanDescriptorSet> AllocateSet();
		void ReleaseSet(const Ref<VulkanDescriptorSet>& set);

		inline Ref<VulkanDescriptorSetLayout> GetLayout() const { return m_Layout; }
	private:
		Ref<VulkanDescriptorSetLayout> m_Layout = nullptr;
		VkDescriptorPool m_Pool = VK_NULL_HANDLE;
	};
}
