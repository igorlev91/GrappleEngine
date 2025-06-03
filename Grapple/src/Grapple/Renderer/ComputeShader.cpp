#include "ComputeShader.h"

#include "Grapple/Renderer/RendererAPI.h"

#include "Grapple/Platform/Vulkan/VulkanComputeShader.h"

namespace Grapple
{
	Grapple_IMPL_ASSET(ComputeShader);
	Grapple_SERIALIZABLE_IMPL(ComputeShader);
	
	ComputeShader::ComputeShader()
		: Asset(AssetType::ComputeShader) {}

	ComputeShader::~ComputeShader() {}

	Ref<ComputeShader> ComputeShader::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanComputeShader>();
		}

		return nullptr;
	}
}