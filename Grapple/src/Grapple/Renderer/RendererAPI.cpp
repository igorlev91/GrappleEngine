#include "RendererAPI.h"

#include "GrappleCore/Assert.h"
#include "Grapple/Platform/OpenGL/OpenGLRendererAPI.h"
#include "Grapple/Platform/Vulkan/VulkanRendererAPI.h"

namespace Grapple
{
	RendererAPI::API s_API = RendererAPI::API::Vulkan;
	Scope<RendererAPI> s_Instance = nullptr;

	void RendererAPI::Create(RendererAPI::API api)
	{
		s_API = api;

		switch (s_API)
		{
		case API::OpenGL:
			s_Instance.reset(new OpenGLRendererAPI());
			break;
		case API::Vulkan:
			s_Instance.reset(new VulkanRendererAPI());
			break;
		default:
			Grapple_CORE_ASSERT(false, "Unsupported rendering API");
		}
	}

	void RendererAPI::Release()
	{
		s_Instance.reset(nullptr);
	}

	const Scope<RendererAPI>& RendererAPI::GetInstance()
	{
		return s_Instance;
	}

	RendererAPI::API RendererAPI::GetAPI()
	{
		return s_API;
	}
}