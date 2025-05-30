#include "VulkanIndexBuffer.h"

namespace Grapple
{
	VulkanIndexBuffer::VulkanIndexBuffer(IndexFormat format, size_t count)
		: m_Format(format), m_Buffer(GPUBufferUsage::Static, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, GetIndexFormatSize(format) * count), m_Count(count)
	{
	}

	VulkanIndexBuffer::VulkanIndexBuffer(IndexFormat format, const MemorySpan& indices)
		: m_Format(format), m_Buffer(GPUBufferUsage::Dynamic, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
	{
		m_Buffer.SetData(indices.GetBuffer(), indices.GetSize(), 0);
		m_Count = indices.GetSize() / IndexBuffer::GetIndexFormatSize(m_Format);
	}

	void Grapple::VulkanIndexBuffer::Bind()
	{
	}

	void Grapple::VulkanIndexBuffer::SetData(const MemorySpan& indices, size_t offset)
	{
		m_Buffer.SetData(indices.GetBuffer(), indices.GetSize(), offset);
	}

	size_t Grapple::VulkanIndexBuffer::GetCount() const
	{
		return m_Count;
	}

	IndexBuffer::IndexFormat Grapple::VulkanIndexBuffer::GetIndexFormat() const
	{
		return m_Format;
	}
}
