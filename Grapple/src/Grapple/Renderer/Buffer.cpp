#include "Buffer.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/OpenGL/OpenGLBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanIndexBuffer.h"

namespace Grapple
{
	Ref<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>();
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
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>();
		default:
			return nullptr;
		}

		vertexBuffer->SetData(data, size);
		return vertexBuffer;
	}
	
	Ref<IndexBuffer> IndexBuffer::Create(IndexFormat format, size_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(format, size);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>();
		}

		return nullptr;
	}
	
	Ref<IndexBuffer> IndexBuffer::Create(IndexBuffer::IndexFormat format, const MemorySpan& indices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(format, indices);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>();
		}

		return nullptr;
	}
}