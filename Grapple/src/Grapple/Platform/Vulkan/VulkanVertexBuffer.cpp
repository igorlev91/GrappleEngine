#include "VulkanVertexBuffer.h"

namespace Grapple
{
	const BufferLayout& VulkanVertexBuffer::GetLayout() const
	{
		return m_Layout;
	}

	void VulkanVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_Layout = layout;
	}

	void VulkanVertexBuffer::Bind()
	{
	}

	void VulkanVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
	}
}
