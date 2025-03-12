#include "VertexArray.h"

#include <Grapple/Renderer/RendererAPI.h>

#include <Grapple/Platform/OpenGL/OpenGLVertexArray.h>

namespace Grapple
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexArray>();
		}
	}
}