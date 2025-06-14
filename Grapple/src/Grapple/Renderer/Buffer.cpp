#include "Buffer.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/Vulkan/VulkanVertexBuffer.h"
#include "Grapple/Platform/Vulkan/VulkanIndexBuffer.h"

namespace Grapple
{
	Ref<VertexBuffer> VertexBuffer::Create(size_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(size);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(size_t size, GPUBufferUsage usage)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(size, usage);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(size_t size, const void* data)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(data, size);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(size_t size, const void* data, Ref<CommandBuffer> commandBuffer)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(data, size, commandBuffer);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
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
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(format, size);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(IndexFormat format, size_t size, GPUBufferUsage usage)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(format, size, usage);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}
	
	Ref<IndexBuffer> IndexBuffer::Create(IndexBuffer::IndexFormat format, const MemorySpan& indices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(format, indices);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(IndexFormat format, const MemorySpan& indices, Ref<CommandBuffer> commandBuffer)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(format, indices, commandBuffer);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}
}