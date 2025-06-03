#include "ComputePipeline.h"

#include "Grapple/Renderer/RendererAPI.h"

#include "Grapple/Platform/Vulkan/VulkanComputePipeline.h"

namespace Grapple
{
	Ref<ComputePipeline> ComputePipeline::Create(const ComputePipelineSpecifications& specifications)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanComputePipeline>(specifications);
		}

		Grapple_CORE_ASSERT(false);
		return nullptr;
	}
}
