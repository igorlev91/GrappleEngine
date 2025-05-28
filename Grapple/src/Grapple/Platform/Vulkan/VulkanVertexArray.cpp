#include "VulkanVertexArray.h"

namespace Grapple
{
	const VertexArray::VertexBuffers& Grapple::VulkanVertexArray::GetVertexBuffers() const
	{
		return m_VertexBuffers;
	}

	void Grapple::VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexbuffer)
	{
		m_IndexBuffer = indexbuffer;
	}

	const Ref<IndexBuffer> Grapple::VulkanVertexArray::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	void Grapple::VulkanVertexArray::Bind() const
	{
	}

	void Grapple::VulkanVertexArray::Unbind() const
	{
	}

	void Grapple::VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, std::optional<uint32_t> baseBinding)
	{
		m_VertexBuffers.push_back(vertexBuffer);
	}
}
