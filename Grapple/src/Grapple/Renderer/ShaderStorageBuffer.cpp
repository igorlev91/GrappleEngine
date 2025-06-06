#include "ShaderStorageBuffer.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/Vulkan/VulkanShaderStorageBuffer.h"

namespace Grapple
{
	Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(size_t size)
	{
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanShaderStorageBuffer>(size);
        }

        return nullptr;
	}
}
