#include "VulkanIndexBuffer.h"

namespace Grapple
{
	void Grapple::VulkanIndexBuffer::Bind()
	{
	}

	void Grapple::VulkanIndexBuffer::SetData(const MemorySpan& indices, size_t offset)
	{
	}

	size_t Grapple::VulkanIndexBuffer::GetCount() const
	{
		return 0;
	}

	IndexBuffer::IndexFormat Grapple::VulkanIndexBuffer::GetIndexFormat() const
	{
		return IndexBuffer::IndexFormat::UInt32;
	}
}
