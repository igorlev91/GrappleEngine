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
			return CreateRef<VulkanVertexBuffer>(size);
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
			return CreateRef<VulkanVertexBuffer>(data, size);
		default:
			return nullptr;
		}

		vertexBuffer->SetData(data, size);
		return vertexBuffer;
	}

	size_t IndexBuffer::GetIndexFormatSize(IndexFormat format)
	{
		switch (format)
		{
		case IndexFormat::UInt16:
			return sizeof(uint16_t);
		case IndexFormat::UInt32:
			return sizeof(uint32_t);
		}

		Grapple_CORE_ASSERT(false);
		return 0;
	}

	Ref<IndexBuffer> IndexBuffer::Create(IndexFormat format, size_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(format, size);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(format, size);
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
			return CreateRef<VulkanIndexBuffer>(format, indices);
		}

		return nullptr;
	}
}