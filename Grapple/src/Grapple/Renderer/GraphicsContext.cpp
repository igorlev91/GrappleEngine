#include "GraphicsContext.h"

#include "Grapple/Platform/OpenGL/OpenGLGraphicsContext.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	Scope<GraphicsContext> GraphicsContext::Create(void* windowHandle)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLGraphicsContext>((GLFWwindow*) windowHandle);
		}
		return nullptr;
	}
}