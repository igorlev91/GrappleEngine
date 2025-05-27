#include "GraphicsContext.h"

#include "Grapple/Platform/OpenGL/OpenGLGraphicsContext.h"
#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	static Scope<GraphicsContext> s_Instance = nullptr;

	GraphicsContext& GraphicsContext::GetInstance()
	{
		return *s_Instance;
	}

	void GraphicsContext::Create(void* windowHandle)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			s_Instance = CreateScope<OpenGLGraphicsContext>((GLFWwindow*) windowHandle);
			break;
		case RendererAPI::API::Vulkan:
			s_Instance = CreateScope<VulkanContext>((GLFWwindow*) windowHandle);
			break;
		default:
			Grapple_CORE_ASSERT(false);
		}

		s_Instance->Initialize();
	}

	void GraphicsContext::Shutdown()
	{
		s_Instance->Release();
		s_Instance = nullptr;
	}
}