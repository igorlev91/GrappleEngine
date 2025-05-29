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
		~VulkanVertexBuffer();

		const BufferLayout& GetLayout() const override;
		void SetLayout(const BufferLayout& layout) override;
		void Bind() override;
		void SetData(const void* data, size_t size, size_t offset) override;

		inline VkBuffer GetHandle() const { return m_Buffer.GetBuffer(); }
	private:
		VulkanBuffer m_Buffer;
		BufferLayout m_Layout;
	};
}
