#pragma once

#include "Grapple/Renderer/VertexArray.h"

namespace Grapple
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();
	public:
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, std::optional<uint32_t> baseBinding) override;
		virtual const VertexBuffers& GetVertexBuffers() const override { return m_VertexBuffers; }

		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexbuffer) override;
		virtual const Ref<IndexBuffer> GetIndexBuffer() const override;
	private:
		uint32_t m_Id;
		uint32_t m_VertexBufferIndex;
		Ref<IndexBuffer> m_IndexBuffer;

		VertexBuffers m_VertexBuffers;
	};
}