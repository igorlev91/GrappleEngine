#pragma once

#include "Grapple/Renderer/Buffer.h"
#include "Grapple/Platform/Vulkan/VulkanBuffer.h"

namespace Grapple
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(IndexFormat format, size_t count);
		VulkanIndexBuffer(IndexFormat format, const MemorySpan& indices);

		void Bind() override;
		void SetData(const MemorySpan& indices, size_t offset) override;
		size_t GetCount() const override;
		IndexFormat GetIndexFormat() const override;

		inline VkBuffer GetHandle() const { return m_Buffer.GetBuffer(); }
	private:
		size_t m_Count = 0;
		IndexBuffer::IndexFormat m_Format;
		VulkanBuffer m_Buffer;
	};
}
