#include "RendererAPI.h"

#include "GrappleCore/Assert.h"
#include "Grapple/Platform/OpenGL/OpenGLRendererAPI.h"
#include "Grapple/Platform/Vulkan/VulkanRendererAPI.h"

namespace Grapple
{
	RendererAPI::API s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case API::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		case API::Vulkan:
			return CreateScope<VulkanRendererAPI>();
		default:
			Grapple_CORE_ASSERT(false, "Unsupported rendering API");
		}

		return nullptr;
	}

	RendererAPI::API RendererAPI::GetAPI()
	{
		return s_API;
	}
}