#pragma once

#include <Grapple/Renderer/Buffer.h>

#include <glad/glad.h>

namespace Grapple
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer();
		~OpenGLVertexBuffer();
	public:
		virtual void Bind() override;
		virtual void SetData(const void* data, size_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t m_Id;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer();
		~OpenGLIndexBuffer();
	public:
		virtual void Bind() override;
		virtual void SetData(const void* indices, size_t size) override;
	private:
		uint32_t m_Id;
	};
}