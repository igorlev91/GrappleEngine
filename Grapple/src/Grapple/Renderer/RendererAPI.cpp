#include "RendererAPI.h"

#include "Grapple/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Grapple
{
	RendererAPI::API s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case API::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		}
	}

	RendererAPI::API RendererAPI::GetAPI()
	{
		return s_API;
	}
}