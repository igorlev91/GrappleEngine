#include "UniformBuffer.h"

#include "Grapple/Renderer/RendererAPI.h"

#include "Grapple/Platform/Vulkan/VulkanUniformBuffer.h"

namespace Grapple
{
    Ref<UniformBuffer> Grapple::UniformBuffer::Create(size_t size, uint32_t binding)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::Vulkan:
            return CreateRef<VulkanUniformBuffer>(size);
        }

        return nullptr;
    }
}
