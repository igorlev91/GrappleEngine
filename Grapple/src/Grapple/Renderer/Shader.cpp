#include "Shader.h"

#include "Grapple/Platform/OpenGL/OpenGLShader.h"
#include "Grapple/Renderer/RendererAPI.h"

namespace Grapple
{
	Grapple_IMPL_ASSET(Shader);
	Grapple_SERIALIZABLE_IMPL(Shader);

	Ref<Shader> Shader::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>();
		}

		return nullptr;
	}
}
