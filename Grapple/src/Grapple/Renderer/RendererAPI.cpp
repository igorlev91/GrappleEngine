#include "RendererAPI.h"

#include "GrappleCore/Assert.h"

namespace Grapple
{
	RendererAPI::API s_API = RendererAPI::API::Vulkan;

	void RendererAPI::Create(RendererAPI::API api)
	{
		s_API = api;
	}

	RendererAPI::API RendererAPI::GetAPI()
	{
		return s_API;
	}
}