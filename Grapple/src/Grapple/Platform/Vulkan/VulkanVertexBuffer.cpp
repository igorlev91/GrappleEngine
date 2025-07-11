#include "VulkanVertexBuffer.h"

namespace Grapple
{
	VulkanVertexBuffer::VulkanVertexBuffer(size_t size)
		: m_Buffer(
			GPUBufferUsage::Dynamic,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VulkanBuffer::PipelineDependecy(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT),
			size)
	{
	}

	VulkanVertexBuffer::VulkanVertexBuffer(size_t size, GPUBufferUsage usage)
		: m_Buffer(
			usage,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VulkanBuffer::PipelineDependecy(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT),
			size) {}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, size_t size)
		: m_Buffer(
			GPUBufferUsage::Static,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VulkanBuffer::PipelineDependecy(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT),
			size)
	{
		m_Buffer.SetData(data, size, 0);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, size_t size, Ref<CommandBuffer> commandBuffer)
		: m_Buffer(
			GPUBufferUsage::Static,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VulkanBuffer::PipelineDependecy(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT),
			size)
	{
		m_Buffer.SetData(MemorySpan::FromRawBytes(const_cast<void*>(data), size), 0, commandBuffer);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
	}

	void VulkanVertexBuffer::SetData(const void* data, size_t size, size_t offset)
	{
		m_Buffer.SetData(data, size, offset);
	}

	void VulkanVertexBuffer::SetData(MemorySpan data, size_t offset, Ref<CommandBuffer> commandBuffer)
	{
		m_Buffer.SetData(data, offset, commandBuffer);
	}

	void VulkanVertexBuffer::SetDebugName(std::string_view debugName)
	{
		m_Buffer.SetDebugName(debugName);
	}

	const std::string& VulkanVertexBuffer::GetDebugName() const
	{
		return m_Buffer.GetDebugName();
	}
}
