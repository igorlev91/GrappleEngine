#include "GPUTimer.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/Vulkan/VulkanGPUTimer.h"

namespace Grapple
{
	Ref<GPUTimer> GPUTimer::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanGPUTimer>();
		}

		return nullptr;
	}
}
