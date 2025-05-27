#include "GraphicsContext.h"

#include "Grapple/Platform/OpenGL/OpenGLGraphicsContext.h"
#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle)
	{
		VulkanContext context((GLFWwindow*)windowHandle);
		context.Initialize();

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLGraphicsContext>((GLFWwindow*) windowHandle);
		}
		return nullptr;
	}
}