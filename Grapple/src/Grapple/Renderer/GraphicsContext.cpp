#include "GraphicsContext.h"

#include "Grapple/Platform/Vulkan/VulkanContext.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	Scope<GraphicsContext> s_Instance = nullptr;

	GraphicsContext& GraphicsContext::GetInstance()
	{
		return *s_Instance;
	}

	bool GraphicsContext::IsInitialized()
	{
		return s_Instance != nullptr;
	}

	void GraphicsContext::Create(Ref<Window> window)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::Vulkan:
			s_Instance = CreateScope<VulkanContext>(window);
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
