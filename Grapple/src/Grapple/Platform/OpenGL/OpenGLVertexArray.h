#pragma once

#include <Grapple/Renderer/VertexArray.h>

namespace Grapple
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();
	public:
		virtual void Bind() override;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;

		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexbuffer) override;
		virtual const Ref<IndexBuffer> GetIndexBuffer() const override;
	private:
		uint32_t m_Id;
		uint32_t m_VertexBufferIndex;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}