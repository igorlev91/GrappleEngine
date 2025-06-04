#pragma once

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Platform/Vulkan/VulkanBuffer.h"

#include <vulkan/vulkan.h>

namespace Grapple
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(size_t size);
		VulkanVertexBuffer(const void* data, size_t size);
		VulkanVertexBuffer(const void* data, size_t size, Ref<CommandBuffer> commandBuffer);
		~VulkanVertexBuffer();

		void Bind() override;
		void SetData(const void* data, size_t size, size_t offset) override;
		void SetData(MemorySpan data, size_t offset, Ref<CommandBuffer> commandBuffer) override;

		inline VkBuffer GetHandle() const { return m_Buffer.GetBuffer(); }
	private:
		VulkanBuffer m_Buffer;
	};
}
