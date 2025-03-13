#pragma once

#include <Grapple/Renderer/Buffer.h>

#include <glad/glad.h>

namespace Grapple
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(size_t size);
		OpenGLVertexBuffer(size_t size, const void* data);
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
		OpenGLIndexBuffer(size_t count);
		OpenGLIndexBuffer(size_t count, const void* data);
		~OpenGLIndexBuffer();
	public:
		virtual void Bind() override;
		virtual void SetData(const void* indices, size_t count) override;
	private:
		uint32_t m_Id;
	};
}