#include "FrameBuffer.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/Vulkan/VulkanFrameBuffer.h"

namespace Grapple
{
	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecifications& specifications)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanFrameBuffer>(specifications);
		}

		return nullptr;
	}
}