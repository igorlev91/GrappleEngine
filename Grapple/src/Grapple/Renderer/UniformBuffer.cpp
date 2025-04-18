#include "UniformBuffer.h"

#include "Grapple/Renderer/RendererAPI.h"

#include "Grapple/Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Grapple
{
    Ref<UniformBuffer> Grapple::UniformBuffer::Create(size_t size, uint32_t binding)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLUniformBuffer>(size, binding);
        }

        return nullptr;
    }
}
