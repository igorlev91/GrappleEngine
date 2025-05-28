#include "VertexArray.h"

#include "Grapple/Renderer/RendererAPI.h"
#include "Grapple/Platform/OpenGL/OpenGLVertexArray.h"
#include "Grapple/Platform/Vulkan/VulkanVertexArray.h"

namespace Grapple
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexArray>();
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexArray>();
		}

		return nullptr;
	}
}