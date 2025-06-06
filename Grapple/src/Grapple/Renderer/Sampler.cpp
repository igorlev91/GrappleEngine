#include "Sampler.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/Vulkan/VulkanSampler.h"

namespace Grapple
{
	Ref<Sampler> Sampler::Create(const SamplerSpecifications& specifications)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanSampler>(specifications);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}
}
