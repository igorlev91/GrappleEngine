#include "ShaderStorageBuffer.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/OpenGL/OpenGLShaderStorageBuffer.h"

namespace Grapple
{
	Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(uint32_t binding)
	{
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLShaderStorageBuffer>(binding);
        }

        return nullptr;
	}
}
