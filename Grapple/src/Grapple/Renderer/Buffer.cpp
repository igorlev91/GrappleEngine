#include "Buffer.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/OpenGL/OpenGLBuffer.h"

namespace Grapple
{
	Ref<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);
		}

		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(size_t size, const void* data)
	{
		Ref<VertexBuffer> vertexBuffer = nullptr;

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			vertexBuffer = CreateRef<OpenGLVertexBuffer>(size, data);
			break;
		default:
			return nullptr;
		}

		vertexBuffer->SetData(data, size);
		return vertexBuffer;
	}
	
	Ref<IndexBuffer> IndexBuffer::Create(size_t count)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(count);
		}

		return nullptr;
	}
	
	Ref<IndexBuffer> IndexBuffer::Create(size_t count, const void* data)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(count, data);
		}

		return nullptr;
	}
}